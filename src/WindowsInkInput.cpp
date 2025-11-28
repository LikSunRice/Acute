#ifdef PLATFORM_WINDOWS

#include "WindowsInkInput.h"
#include <windows.h>
#include <winuser.h>
#include <chrono>
#include <iostream>
#include <iomanip>

// Windows Pointer API (Windows 8+)
// These are defined in winuser.h for Windows 8+, but we define them for compatibility
#ifndef WM_POINTERDOWN
#define WM_POINTERDOWN 0x0246
#define WM_POINTERUPDATE 0x0245
#define WM_POINTERUP 0x0247
#define GET_POINTERID_WPARAM(wParam) (LOWORD(wParam))
#endif

// Load pointer API functions dynamically (for Windows 7 compatibility)
typedef BOOL (WINAPI *GetPointerInfoProc)(UINT32, POINTER_INFO*);
typedef BOOL (WINAPI *GetPointerPenInfoProc)(UINT32, POINTER_PEN_INFO*);

static GetPointerInfoProc g_GetPointerInfo = nullptr;
static GetPointerPenInfoProc g_GetPointerPenInfo = nullptr;
static bool g_PointerAPIInitialized = false;

static void InitializePointerAPI() {
    if (g_PointerAPIInitialized) return;
    
    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    if (user32) {
        g_GetPointerInfo = (GetPointerInfoProc)GetProcAddress(user32, "GetPointerInfo");
        g_GetPointerPenInfo = (GetPointerPenInfoProc)GetProcAddress(user32, "GetPointerPenInfo");
    }
    
    g_PointerAPIInitialized = true;
}

namespace Acute {

// PIMPL implementation
class WindowsInkInput::Impl {
public:
    HWND hwnd;
};

WindowsInkInput::WindowsInkInput()
    : m_penAvailable(false)
    , m_penDown(false)
    , m_lastTimestamp(0)
    , m_impl(std::make_unique<Impl>())
{
    m_impl->hwnd = nullptr;
}

WindowsInkInput::~WindowsInkInput() {
    shutdown();
}

uint64_t WindowsInkInput::getCurrentTime() const {
    auto now = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
    return static_cast<uint64_t>(duration.count());
}

float WindowsInkInput::normalizePressure(unsigned int pressure) const {
    // Windows pointer pressure is typically 0-1024
    // Some devices use 0-8192 or 0-4096
    // Normalize to 0.0-1.0 range
    if (pressure == 0) return 0.0f;
    if (pressure <= 1024) {
        return static_cast<float>(pressure) / 1024.0f;
    } else if (pressure <= 4096) {
        return static_cast<float>(pressure) / 4096.0f;
    } else {
        return static_cast<float>(pressure) / 8192.0f;
    }
}

float WindowsInkInput::normalizeTilt(int tilt) const {
    // Windows pointer tilt is typically -90 to +90 degrees
    // Normalize to -1.0 to 1.0 range
    return static_cast<float>(tilt) / 90.0f;
}

void WindowsInkInput::updateVelocity() {
    if (m_lastTimestamp == 0) {
        m_currentInput.velocityX = 0.0f;
        m_currentInput.velocityY = 0.0f;
        return;
    }
    
    uint64_t deltaTime = m_currentInput.timestamp - m_lastTimestamp;
    if (deltaTime > 0) {
        float dt = deltaTime / 1000.0f; // Convert to seconds
        m_currentInput.velocityX = (m_currentInput.x - m_previousInput.x) / dt;
        m_currentInput.velocityY = (m_currentInput.y - m_previousInput.y) / dt;
    }
}

// Window procedure hook to intercept messages before SDL
static WindowsInkInput* g_currentInkInput = nullptr;
static WNDPROC g_originalWndProc = nullptr;

static LRESULT CALLBACK InkWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    // Try to process pointer messages with Windows Ink
    if (g_currentInkInput && (msg >= 0x0245 && msg <= 0x0247)) {
        if (g_currentInkInput->processMessage(hwnd, msg, reinterpret_cast<void*>(wParam), reinterpret_cast<void*>(lParam))) {
            // Windows Ink handled it - don't pass to SDL
            return 0;
        }
    }
    
    // Call original window procedure (SDL's)
    if (g_originalWndProc) {
        return CallWindowProc(g_originalWndProc, hwnd, msg, wParam, lParam);
    }
    
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

bool WindowsInkInput::initialize(void* hwnd) {
    if (!hwnd) {
        std::cout << "[Windows Ink] ERROR: No window handle provided" << std::endl;
        return false;
    }
    
    m_impl->hwnd = static_cast<HWND>(hwnd);
    
    // Subclass the window to intercept messages before SDL
    g_currentInkInput = this;
    g_originalWndProc = (WNDPROC)SetWindowLongPtr(m_impl->hwnd, GWLP_WNDPROC, (LONG_PTR)InkWindowProc);
    
    if (!g_originalWndProc) {
        std::cout << "[Windows Ink] WARNING: Failed to subclass window procedure" << std::endl;
    } else {
        std::cout << "[Windows Ink] Window subclassed successfully" << std::endl;
    }
    
    // Check if pen/stylus is available
    int penCount = GetSystemMetrics(SM_PENWINDOWS);
    m_penAvailable = (penCount > 0);
    
    // Also check for touch/pen support
    if (!m_penAvailable) {
        int digitizerStatus = GetSystemMetrics(SM_DIGITIZER);
        m_penAvailable = (digitizerStatus & NID_INTEGRATED_TOUCH) != 0 ||
                        (digitizerStatus & NID_EXTERNAL_TOUCH) != 0 ||
                        (digitizerStatus & NID_INTEGRATED_PEN) != 0 ||
                        (digitizerStatus & NID_EXTERNAL_PEN) != 0;
        
        std::cout << "[Windows Ink] Digitizer status: 0x" << std::hex << digitizerStatus << std::dec << std::endl;
    }
    
    if (m_penAvailable) {
        std::cout << "[Windows Ink] Pen/tablet detected!" << std::endl;
    } else {
        std::cout << "[Windows Ink] No pen/tablet detected - will use mouse input" << std::endl;
    }
    
    // Initialize pointer API
    InitializePointerAPI();
    if (g_GetPointerInfo && g_GetPointerPenInfo) {
        std::cout << "[Windows Ink] Pointer API functions loaded successfully" << std::endl;
    } else {
        std::cout << "[Windows Ink] WARNING: Pointer API functions not available" << std::endl;
    }
    
    return true;
}

bool WindowsInkInput::processMessage(void* hwnd, unsigned int msg, void* wParam, void* lParam) {
    if (!m_penAvailable) {
        return false;
    }
    
    InitializePointerAPI();
    
    if (!g_GetPointerInfo || !g_GetPointerPenInfo) {
        return false; // Pointer API not available
    }
    
    HWND windowHandle = static_cast<HWND>(hwnd);
    UINT message = static_cast<UINT>(msg);
    WPARAM wparam = reinterpret_cast<WPARAM>(wParam);
    
    // Handle pointer messages (Windows 8+)
    // WM_POINTERDOWN = 0x0246, WM_POINTERUPDATE = 0x0245, WM_POINTERUP = 0x0247
    if (message >= 0x0245 && message <= 0x0247) {
        static int messageCount = 0;
        if (messageCount++ == 0) {
            std::cout << "[Windows Ink] First pointer message received! Message: 0x" 
                      << std::hex << message << std::dec << std::endl;
        }
        UINT32 pointerId = GET_POINTERID_WPARAM(wparam);
        
        POINTER_INFO pointerInfo = {};
        if (g_GetPointerInfo(pointerId, &pointerInfo) == FALSE) {
            return false;
        }
        
        // Check if this is a pen/stylus pointer
        if (pointerInfo.pointerType != PT_PEN) {
            static int nonPenCount = 0;
            if (nonPenCount++ < 3) {
                std::cout << "[Windows Ink] Ignoring non-pen pointer type: " 
                          << pointerInfo.pointerType << std::endl;
            }
            return false; // Not a pen, ignore
        }
        
        static int penMessageCount = 0;
        if (penMessageCount++ == 0) {
            std::cout << "[Windows Ink] First PEN pointer message detected!" << std::endl;
        }
        
        POINT clientPoint = pointerInfo.ptPixelLocation;
        ScreenToClient(windowHandle, &clientPoint);
        
        // Get pointer properties
        POINTER_PEN_INFO penInfo = {};
        if (g_GetPointerPenInfo(pointerId, &penInfo) == FALSE) {
            return false;
        }
        
        // Update input point
        m_previousInput = m_currentInput;
        m_currentInput.x = static_cast<float>(clientPoint.x);
        m_currentInput.y = static_cast<float>(clientPoint.y);
        m_currentInput.timestamp = getCurrentTime();
        
        // Extract pressure (0-1024 range, normalize to 0.0-1.0)
        // Check if pressure is available in the pen mask
        bool pressureFromMask = (penInfo.penMask & PEN_MASK_PRESSURE) != 0;
        
        if (pressureFromMask) {
            // Pressure is available - normalize it
            m_currentInput.pressure = normalizePressure(penInfo.pressure);
            // Debug output
            static int debugCounter = 0;
            if (debugCounter++ % 10 == 0) { // Print every 10th sample to avoid spam
                std::cout << "[Windows Ink] Pressure: raw=" << penInfo.pressure 
                          << ", normalized=" << m_currentInput.pressure 
                          << ", mask=0x" << std::hex << penInfo.penMask << std::dec << std::endl;
            }
        } else {
            // Pressure not available in mask, but try to use the value anyway
            // Some devices report pressure even if mask doesn't indicate it
            if (penInfo.pressure > 0 && penInfo.pressure <= 1024) {
                m_currentInput.pressure = normalizePressure(penInfo.pressure);
                static int debugCounter = 0;
                if (debugCounter++ % 10 == 0) {
                    std::cout << "[Windows Ink] Pressure (no mask): raw=" << penInfo.pressure 
                              << ", normalized=" << m_currentInput.pressure << std::endl;
                }
            } else {
                // Fallback: estimate pressure from contact state
                // Light touch = lower pressure, firm touch = higher pressure
                m_currentInput.pressure = 0.7f; // Default medium-high pressure
                static int debugCounter = 0;
                if (debugCounter++ % 10 == 0) {
                    std::cout << "[Windows Ink] WARNING: No pressure data! Using default: " 
                              << m_currentInput.pressure << std::endl;
                    std::cout << "  - Raw pressure value: " << penInfo.pressure << std::endl;
                    std::cout << "  - Pen mask: 0x" << std::hex << penInfo.penMask << std::dec << std::endl;
                    std::cout << "  - Pen flags: 0x" << std::hex << penInfo.penFlags << std::dec << std::endl;
                }
            }
        }
        
        // Extract tilt (if available)
        if (penInfo.tiltX != 0 || penInfo.tiltY != 0) {
            m_currentInput.tiltX = normalizeTilt(penInfo.tiltX);
            m_currentInput.tiltY = normalizeTilt(penInfo.tiltY);
        }
        
        // Extract rotation (barrel rotation, if available)
        // Note: Windows pointer API doesn't directly provide rotation
        // This would require Windows Ink API for full support
        m_currentInput.rotation = 0.0f;
        
        // Update velocity
        updateVelocity();
        m_lastTimestamp = m_currentInput.timestamp;
        
        // Handle different message types
        bool triggerCallback = false;
        if (message == 0x0246) { // WM_POINTERDOWN
            m_penDown = true;
            m_lastTimestamp = 0; // Reset for velocity calculation
            triggerCallback = true;
        } else if (message == 0x0245) { // WM_POINTERUPDATE
            if (m_penDown) {
                triggerCallback = true;
            }
        } else if (message == 0x0247) { // WM_POINTERUP
            m_penDown = false;
            m_currentInput.pressure = 0.0f;
            triggerCallback = true;
        }
        
        if (triggerCallback && m_callback) {
            m_callback(m_currentInput, m_penDown);
        }
        
        return true; // Handled
    }
    
    return false; // Not handled
}

void WindowsInkInput::setInputCallback(InputCallback callback) {
    m_callback = callback;
}

void WindowsInkInput::shutdown() {
    // Restore original window procedure
    if (m_impl->hwnd && g_originalWndProc) {
        SetWindowLongPtr(m_impl->hwnd, GWLP_WNDPROC, (LONG_PTR)g_originalWndProc);
        g_originalWndProc = nullptr;
    }
    
    g_currentInkInput = nullptr;
    m_penAvailable = false;
    m_penDown = false;
    m_impl->hwnd = nullptr;
}

} // namespace Acute

#endif // PLATFORM_WINDOWS

