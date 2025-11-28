#include "Window.h"
#include <GL/glew.h>
#include <iostream>

#ifdef PLATFORM_WINDOWS
#include <SDL2/SDL_syswm.h>
#endif

namespace Acute {

Window::Window(const std::string& title, int width, int height)
    : m_title(title)
    , m_width(width)
    , m_height(height)
    , m_window(nullptr)
    , m_glContext(nullptr)
    , m_shouldClose(false)
{
}

Window::~Window() {
    if (m_glContext) {
        SDL_GL_DeleteContext(m_glContext);
    }
    if (m_window) {
        SDL_DestroyWindow(m_window);
    }
}

bool Window::initialize() {
    // Set OpenGL attributes
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    
    // Create window
    m_window = SDL_CreateWindow(
        m_title.c_str(),
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        m_width,
        m_height,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
    );
    
    if (!m_window) {
        std::cerr << "Failed to create SDL window: " << SDL_GetError() << std::endl;
        return false;
    }
    
    // Create OpenGL context
    m_glContext = SDL_GL_CreateContext(m_window);
    if (!m_glContext) {
        std::cerr << "Failed to create OpenGL context: " << SDL_GetError() << std::endl;
        return false;
    }
    
    // Initialize GLEW
    glewExperimental = GL_TRUE;
    GLenum glewError = glewInit();
    if (glewError != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW: " << glewGetErrorString(glewError) << std::endl;
        return false;
    }
    
    // Enable VSync
    SDL_GL_SetSwapInterval(1);
    
    // Set viewport
    glViewport(0, 0, m_width, m_height);
    
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    
    return true;
}

void Window::swapBuffers() {
    SDL_GL_SwapWindow(m_window);
}

void* Window::getNativeHandle() const {
#ifdef PLATFORM_WINDOWS
    SDL_SysWMinfo wmInfo;
    SDL_VERSION(&wmInfo.version);
    if (SDL_GetWindowWMInfo(m_window, &wmInfo)) {
        return static_cast<void*>(wmInfo.info.win.window);
    }
#endif
    return nullptr;
}

} // namespace Acute


