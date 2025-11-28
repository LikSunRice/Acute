#include "Canvas.h"
#include "Shader.h"
#include <iostream>
#include <cmath>

namespace Acute {

Canvas::Canvas(int width, int height)
    : m_width(width)
    , m_height(height)
    , m_framebuffer(0)
    , m_canvasTexture(0)
    , m_dabVAO(0)
    , m_dabVBO(0)
    , m_screenVAO(0)
    , m_screenVBO(0)
    , m_brushTexture(0)
{
}

Canvas::~Canvas() {
    if (m_dabVAO) glDeleteVertexArrays(1, &m_dabVAO);
    if (m_dabVBO) glDeleteBuffers(1, &m_dabVBO);
    if (m_screenVAO) glDeleteVertexArrays(1, &m_screenVAO);
    if (m_screenVBO) glDeleteBuffers(1, &m_screenVBO);
    if (m_framebuffer) glDeleteFramebuffers(1, &m_framebuffer);
    if (m_canvasTexture) glDeleteTextures(1, &m_canvasTexture);
    if (m_brushTexture) glDeleteTextures(1, &m_brushTexture);
}

bool Canvas::initialize() {
    if (!initializeShaders()) {
        return false;
    }
    
    if (!initializeGeometry()) {
        return false;
    }
    
    createBrushTexture();
    
    if (!createFramebuffer()) {
        return false;
    }
    
    clear();
    
    return true;
}

bool Canvas::initializeShaders() {
    // Dab shader for rendering brush strokes
    m_dabShader = std::make_unique<Shader>();
    std::string dabVertexSource = R"(
        #version 330 core
        layout (location = 0) in vec2 aPos;
        layout (location = 1) in vec2 aTexCoord;
        
        out vec2 TexCoord;
        
        uniform mat4 projection;
        uniform vec2 position;
        uniform float size;
        uniform float rotation;
        
        void main() {
            // Rotate and scale
            float c = cos(radians(rotation));
            float s = sin(radians(rotation));
            mat2 rot = mat2(c, -s, s, c);
            vec2 scaled = aPos * size;
            vec2 rotated = rot * scaled;
            vec2 finalPos = position + rotated;
            
            gl_Position = projection * vec4(finalPos, 0.0, 1.0);
            TexCoord = aTexCoord;
        }
    )";
    
    std::string dabFragmentSource = R"(
        #version 330 core
        in vec2 TexCoord;
        out vec4 FragColor;
        
        uniform sampler2D brushTexture;
        uniform vec3 color;
        uniform float opacity;
        uniform float hardness;
        
        void main() {
            float dist = length(TexCoord - vec2(0.5));
            float alpha = 1.0 - smoothstep(0.5 - hardness * 0.5, 0.5, dist);
            alpha *= texture(brushTexture, TexCoord).r;
            alpha *= opacity;
            FragColor = vec4(color, alpha);
        }
    )";
    
    if (!m_dabShader->loadFromSource(dabVertexSource, dabFragmentSource)) {
        std::cerr << "Failed to load dab shader" << std::endl;
        return false;
    }
    
    // Screen shader for displaying the canvas
    m_screenShader = std::make_unique<Shader>();
    std::string screenVertexSource = R"(
        #version 330 core
        layout (location = 0) in vec2 aPos;
        layout (location = 1) in vec2 aTexCoord;
        
        out vec2 TexCoord;
        
        void main() {
            gl_Position = vec4(aPos, 0.0, 1.0);
            TexCoord = aTexCoord;
        }
    )";
    
    std::string screenFragmentSource = R"(
        #version 330 core
        in vec2 TexCoord;
        out vec4 FragColor;
        
        uniform sampler2D screenTexture;
        
        void main() {
            FragColor = texture(screenTexture, TexCoord);
        }
    )";
    
    if (!m_screenShader->loadFromSource(screenVertexSource, screenFragmentSource)) {
        std::cerr << "Failed to load screen shader" << std::endl;
        return false;
    }
    
    return true;
}

bool Canvas::initializeGeometry() {
    // Quad for rendering dabs (-0.5 to 0.5)
    float dabVertices[] = {
        // positions    // texcoords
        -0.5f, -0.5f,   0.0f, 0.0f,
         0.5f, -0.5f,   1.0f, 0.0f,
         0.5f,  0.5f,   1.0f, 1.0f,
        -0.5f, -0.5f,   0.0f, 0.0f,
         0.5f,  0.5f,   1.0f, 1.0f,
        -0.5f,  0.5f,   0.0f, 1.0f
    };
    
    glGenVertexArrays(1, &m_dabVAO);
    glGenBuffers(1, &m_dabVBO);
    
    glBindVertexArray(m_dabVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_dabVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(dabVertices), dabVertices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    // Screen quad (-1 to 1)
    float screenVertices[] = {
        // positions    // texcoords
        -1.0f, -1.0f,   0.0f, 0.0f,
         1.0f, -1.0f,   1.0f, 0.0f,
         1.0f,  1.0f,   1.0f, 1.0f,
        -1.0f, -1.0f,   0.0f, 0.0f,
         1.0f,  1.0f,   1.0f, 1.0f,
        -1.0f,  1.0f,   0.0f, 1.0f
    };
    
    glGenVertexArrays(1, &m_screenVAO);
    glGenBuffers(1, &m_screenVBO);
    
    glBindVertexArray(m_screenVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_screenVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(screenVertices), screenVertices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glBindVertexArray(0);
    
    return true;
}

void Canvas::createBrushTexture() {
    const int size = 256;
    std::vector<unsigned char> data(size * size);
    
    // Create a radial gradient
    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
            float dx = (x - size * 0.5f) / (size * 0.5f);
            float dy = (y - size * 0.5f) / (size * 0.5f);
            float dist = std::sqrt(dx * dx + dy * dy);
            float value = std::max(0.0f, 1.0f - dist);
            data[y * size + x] = static_cast<unsigned char>(value * 255);
        }
    }
    
    glGenTextures(1, &m_brushTexture);
    glBindTexture(GL_TEXTURE_2D, m_brushTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, size, size, 0, GL_RED, GL_UNSIGNED_BYTE, data.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

bool Canvas::createFramebuffer() {
    // Create framebuffer
    glGenFramebuffers(1, &m_framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
    
    // Create texture for canvas
    glGenTextures(1, &m_canvasTexture);
    glBindTexture(GL_TEXTURE_2D, m_canvasTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    // Attach texture to framebuffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_canvasTexture, 0);
    
    // Check framebuffer status
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Framebuffer is not complete!" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return false;
    }
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return true;
}

void Canvas::clear(float r, float g, float b, float a) {
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Canvas::drawDab(const BrushDab& dab) {
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
    
    // Enable blending for alpha compositing
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Use dab shader
    m_dabShader->use();
    
    // Set up orthographic projection
    float projection[16] = {
        2.0f / m_width, 0.0f, 0.0f, 0.0f,
        0.0f, -2.0f / m_height, 0.0f, 0.0f,
        0.0f, 0.0f, -1.0f, 0.0f,
        -1.0f, 1.0f, 0.0f, 1.0f
    };
    m_dabShader->setMat4("projection", projection);
    
    // Set uniforms
    m_dabShader->setVec2("position", dab.x, dab.y);
    m_dabShader->setFloat("size", dab.size);
    m_dabShader->setFloat("rotation", dab.rotation);
    m_dabShader->setVec3("color", dab.r, dab.g, dab.b);
    m_dabShader->setFloat("opacity", dab.opacity * dab.flow);
    m_dabShader->setFloat("hardness", dab.hardness);
    
    // Bind brush texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_brushTexture);
    m_dabShader->setInt("brushTexture", 0);
    
    // Draw quad
    glBindVertexArray(m_dabVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    
    glDisable(GL_BLEND);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Canvas::drawDabs(const std::vector<BrushDab>& dabs) {
    for (const auto& dab : dabs) {
        drawDab(dab);
    }
}

void Canvas::render() {
    // Render canvas texture to screen
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    m_screenShader->use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_canvasTexture);
    m_screenShader->setInt("screenTexture", 0);
    
    glBindVertexArray(m_screenVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void Canvas::resize(int width, int height) {
    m_width = width;
    m_height = height;
    
    // Recreate framebuffer with new size
    if (m_framebuffer) {
        glDeleteFramebuffers(1, &m_framebuffer);
        glDeleteTextures(1, &m_canvasTexture);
    }
    
    createFramebuffer();
    clear();
}

} // namespace Acute


