#pragma once

#include <string>
#include <GL/glew.h>

namespace Acute {

class Shader {
public:
    Shader();
    ~Shader();
    
    // Load and compile shaders from source code
    bool loadFromSource(const std::string& vertexSource, const std::string& fragmentSource);
    
    // Load shaders from files
    bool loadFromFile(const std::string& vertexPath, const std::string& fragmentPath);
    
    // Use this shader program
    void use() const;
    
    // Utility functions for setting uniforms
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setVec2(const std::string& name, float x, float y) const;
    void setVec3(const std::string& name, float x, float y, float z) const;
    void setVec4(const std::string& name, float x, float y, float z, float w) const;
    void setMat4(const std::string& name, const float* value) const;
    
    GLuint getProgram() const { return m_program; }
    
private:
    GLuint m_program;
    
    // Compile a single shader
    GLuint compileShader(GLenum type, const std::string& source);
    
    // Link the program
    bool linkProgram(GLuint vertexShader, GLuint fragmentShader);
};

} // namespace Acute


