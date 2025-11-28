#pragma once

#include "InputTypes.h"
#include <SDL2/SDL.h>
#include <vector>
#include <functional>
#include <memory>

namespace Acute {

// Forward declaration for Windows Ink (defined in WindowsInkInput.h)
#ifdef PLATFORM_WINDOWS
class WindowsInkInput;
#endif

// Callback for input events
using InputCallback = std::function<void(const InputPoint&, bool)>;

class InputManager {
public:
    InputManager();
    ~InputManager();
    
    // Initialize input manager (requires window handle for Windows Ink)
    bool initialize(void* nativeWindowHandle = nullptr);
    
    // Process SDL events and generate input points
    void processEvent(const SDL_Event& event);
    
    // Process Windows messages (for Windows Ink support)
    bool processWindowsMessage(void* hwnd, unsigned int msg, void* wParam, void* lParam);
    
    // Set callback for input events (point, isPressed)
    void setInputCallback(InputCallback callback);
    
    // Get current input state
    const InputPoint& getCurrentInput() const { return m_currentInput; }
    bool isPressed() const { return m_isPressed; }
    
    // Check if pen/stylus is available
    bool isPenAvailable() const;
    
private:
    InputPoint m_currentInput;
    InputPoint m_previousInput;
    bool m_isPressed;
    InputCallback m_callback;
    uint64_t m_lastTimestamp;
    
#ifdef PLATFORM_WINDOWS
    // WindowsInkInput is forward declared at namespace level above
    std::unique_ptr<WindowsInkInput> m_windowsInk;
#endif
    
    // Update velocity based on position change
    void updateVelocity();
    
    // Get current timestamp in milliseconds
    uint64_t getCurrentTime() const;
};

} // namespace Acute

