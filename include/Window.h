#pragma once

#include <SDL2/SDL.h>
#include <string>

namespace Acute {

class Window {
public:
    Window(const std::string& title, int width, int height);
    ~Window();
    
    // Initialize the window
    bool initialize();
    
    // Swap buffers (present frame)
    void swapBuffers();
    
    // Get window dimensions
    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }
    
    // Get SDL window handle
    SDL_Window* getSDLWindow() { return m_window; }
    
    // Get native window handle (for Windows Ink integration)
    void* getNativeHandle() const;
    
    // Check if window should close
    bool shouldClose() const { return m_shouldClose; }
    void setShouldClose(bool value) { m_shouldClose = value; }
    
private:
    std::string m_title;
    int m_width;
    int m_height;
    SDL_Window* m_window;
    SDL_GLContext m_glContext;
    bool m_shouldClose;
};

} // namespace Acute


