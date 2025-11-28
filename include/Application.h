#pragma once

#include <memory>
#include <string>

namespace Acute {

class Window;
class Canvas;
class InputManager;
class BrushEngine;
class Renderer;

class Application {
public:
    Application();
    ~Application();
    
    // Initialize the application
    bool initialize(const std::string& title, int width, int height);
    
    // Run the main loop
    void run();
    
    // Shutdown the application
    void shutdown();
    
private:
    std::unique_ptr<Window> m_window;
    std::unique_ptr<Canvas> m_canvas;
    std::unique_ptr<InputManager> m_inputManager;
    std::unique_ptr<BrushEngine> m_brushEngine;
    std::unique_ptr<Renderer> m_renderer;
    
    bool m_running;
    bool m_strokeActive;  // Track if a stroke is currently active
    
    // Handle events
    void handleEvents();
    
    // Update application state
    void update(float deltaTime);
    
    // Render frame
    void render();
    
    // Setup default brush
    void setupDefaultBrush();
};

} // namespace Acute


