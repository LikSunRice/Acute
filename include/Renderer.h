#pragma once

namespace Acute {

class Renderer {
public:
    Renderer();
    ~Renderer();
    
    // Initialize renderer
    bool initialize();
    
    // Set clear color
    void setClearColor(float r, float g, float b, float a = 1.0f);
    
    // Clear the screen
    void clear();
    
private:
    float m_clearColor[4];
};

} // namespace Acute


