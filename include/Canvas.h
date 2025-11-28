#pragma once

#include "BrushDab.h"
#include <GL/glew.h>
#include <vector>
#include <memory>

namespace Acute {

class Shader;

// Canvas manages the drawing surface and compositing
class Canvas {
public:
    Canvas(int width, int height);
    ~Canvas();
    
    // Initialize OpenGL resources
    bool initialize();
    
    // Clear the canvas
    void clear(float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f);
    
    // Draw a single dab onto the canvas
    void drawDab(const BrushDab& dab);
    
    // Draw multiple dabs
    void drawDabs(const std::vector<BrushDab>& dabs);
    
    // Render the canvas to the screen
    void render();
    
    // Resize the canvas
    void resize(int width, int height);
    
    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }
    
private:
    int m_width;
    int m_height;
    
    // Framebuffer for canvas rendering
    GLuint m_framebuffer;
    GLuint m_canvasTexture;
    
    // Brush rendering resources
    GLuint m_dabVAO;
    GLuint m_dabVBO;
    std::unique_ptr<Shader> m_dabShader;
    
    // Screen quad for displaying canvas
    GLuint m_screenVAO;
    GLuint m_screenVBO;
    std::unique_ptr<Shader> m_screenShader;
    
    // Brush texture (circular gradient)
    GLuint m_brushTexture;
    
    // Initialize shaders
    bool initializeShaders();
    
    // Initialize geometry
    bool initializeGeometry();
    
    // Create brush texture
    void createBrushTexture();
    
    // Create framebuffer
    bool createFramebuffer();
};

} // namespace Acute


