#include "InputManager.h"
#include <chrono>

#ifdef PLATFORM_WINDOWS
// Include the full definition for WindowsInkInput
#include "WindowsInkInput.h"
#endif

namespace Acute {

InputManager::InputManager()
    : m_isPressed(false)
    , m_lastTimestamp(0)
#ifdef PLATFORM_WINDOWS
    , m_windowsInk(nullptr)
#endif
{
}

InputManager::~InputManager() = default;

bool InputManager::initialize(void* nativeWindowHandle) {
#ifdef PLATFORM_WINDOWS
    if (nativeWindowHandle) {
        // WindowsInkInput is now fully defined (included in .cpp)
        m_windowsInk = std::make_unique<WindowsInkInput>();
        if (m_windowsInk && m_windowsInk->initialize(nativeWindowHandle)) {
            m_windowsInk->setInputCallback(m_callback);
            return true;
        }
        m_windowsInk.reset();
    }
#endif
    return true; // Mouse input still works without Windows Ink
}

bool InputManager::processWindowsMessage(void* hwnd, unsigned int msg, void* wParam, void* lParam) {
#ifdef PLATFORM_WINDOWS
    if (m_windowsInk) {
        // WindowsInkInput is now fully defined (included in .cpp)
        return m_windowsInk->processMessage(hwnd, msg, wParam, lParam);
    }
#endif
    return false;
}

bool InputManager::isPenAvailable() const {
#ifdef PLATFORM_WINDOWS
    // WindowsInkInput is now fully defined (included in .cpp)
    return m_windowsInk && m_windowsInk->isPenAvailable();
#else
    return false;
#endif
}

void InputManager::setInputCallback(InputCallback callback) {
    m_callback = callback;
#ifdef PLATFORM_WINDOWS
    // WindowsInkInput is now fully defined (included in .cpp)
    if (m_windowsInk) {
        m_windowsInk->setInputCallback(callback);
    }
#endif
}

uint64_t InputManager::getCurrentTime() const {
    auto now = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
    return static_cast<uint64_t>(duration.count());
}

void InputManager::updateVelocity() {
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

void InputManager::processEvent(const SDL_Event& event) {
    bool triggerCallback = false;
    
    switch (event.type) {
        case SDL_MOUSEBUTTONDOWN:
            if (event.button.button == SDL_BUTTON_LEFT) {
                m_isPressed = true;
                m_currentInput.x = static_cast<float>(event.button.x);
                m_currentInput.y = static_cast<float>(event.button.y);
                m_currentInput.pressure = 1.0f;
                m_currentInput.timestamp = getCurrentTime();
                m_lastTimestamp = 0; // Reset for velocity calculation
                triggerCallback = true;
            }
            break;
            
        case SDL_MOUSEBUTTONUP:
            if (event.button.button == SDL_BUTTON_LEFT) {
                m_isPressed = false;
                m_currentInput.pressure = 0.0f;
                triggerCallback = true;
            }
            break;
            
        case SDL_MOUSEMOTION:
            if (m_isPressed) {
                m_previousInput = m_currentInput;
                m_currentInput.x = static_cast<float>(event.motion.x);
                m_currentInput.y = static_cast<float>(event.motion.y);
                m_currentInput.timestamp = getCurrentTime();
                updateVelocity();
                m_lastTimestamp = m_currentInput.timestamp;
                triggerCallback = true;
            }
            break;
            
        // TODO: Add tablet/stylus event handling
        // SDL2 doesn't have built-in stylus support, would need platform-specific code
        // or additional libraries like Wintab (Windows) or XInput (Linux)
        
        default:
            break;
    }
    
    if (triggerCallback && m_callback) {
        m_callback(m_currentInput, m_isPressed);
    }
}

} // namespace Acute


