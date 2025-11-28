#pragma once

#ifdef PLATFORM_WINDOWS

#include "InputTypes.h"
#include <functional>
#include <memory>

namespace Acute {

// Callback for input events
using InputCallback = std::function<void(const InputPoint&, bool)>;

// Windows Ink input handler for tablet/stylus support
// Uses Windows Runtime APIs to capture pen pressure, tilt, and rotation
class WindowsInkInput {
public:
    WindowsInkInput();
    ~WindowsInkInput();
    
    // Initialize Windows Ink input (requires HWND)
    bool initialize(void* hwnd);
    
    // Process Windows messages for pointer events
    bool processMessage(void* hwnd, unsigned int msg, void* wParam, void* lParam);
    
    // Set callback for input events
    void setInputCallback(InputCallback callback);
    
    // Check if pen/stylus is available
    bool isPenAvailable() const { return m_penAvailable; }
    
    // Shutdown
    void shutdown();
    
private:
    InputCallback m_callback;
    bool m_penAvailable;
    bool m_penDown;
    InputPoint m_currentInput;
    InputPoint m_previousInput;
    uint64_t m_lastTimestamp;
    
    // Internal implementation (PIMPL pattern)
    class Impl;
    std::unique_ptr<Impl> m_impl;
    
    // Update velocity based on position change
    void updateVelocity();
    
    // Get current timestamp in milliseconds
    uint64_t getCurrentTime() const;
    
    // Convert Windows pointer pressure to 0.0-1.0 range
    float normalizePressure(unsigned int pressure) const;
    
    // Convert Windows pointer tilt to -1.0 to 1.0 range
    float normalizeTilt(int tilt) const;
};

} // namespace Acute

#endif // PLATFORM_WINDOWS

