#include "Shader.h"

#include <GL/glew.h>

#include <fstream>
#include <sstream>
#include <stdexcept>

namespace {

std::string readFile(const std::string& path) {
    std::ifstream file(path);
    if (!file) {
        throw std::runtime_error("Failed to open shader file: " + path);
    }
    std::stringstream stream;
    stream << file.rdbuf();
    return stream.str();
}

} // namespace

unsigned int Shader::compile(const std::string& path, unsigned int type) {
    const std::string source = readFile(path);
    const char* src = source.c_str();

    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    int success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char log[1024];
        glGetShaderInfoLog(shader, sizeof(log), nullptr, log);
        throw std::runtime_error("Shader compile error (" + path + "): " + log);
    }
    return shader;
}

Shader::Shader(const std::string& vertPath, const std::string& fragPath) {
    const unsigned int vert = compile(vertPath, GL_VERTEX_SHADER);
    const unsigned int frag = compile(fragPath, GL_FRAGMENT_SHADER);

    m_program = glCreateProgram();
    glAttachShader(m_program, vert);
    glAttachShader(m_program, frag);
    glLinkProgram(m_program);

    int success = 0;
    glGetProgramiv(m_program, GL_LINK_STATUS, &success);
    if (!success) {
        char log[1024];
        glGetProgramInfoLog(m_program, sizeof(log), nullptr, log);
        glDeleteShader(vert);
        glDeleteShader(frag);
        throw std::runtime_error(std::string("Shader link error: ") + log);
    }

    glDeleteShader(vert);
    glDeleteShader(frag);
}

Shader::~Shader() {
    if (m_program != 0) {
        glDeleteProgram(m_program);
    }
}

void Shader::use() const {
    glUseProgram(m_program);
}

void Shader::setMat4(const std::string& name, const glm::mat4& value) const {
    const int location = glGetUniformLocation(m_program, name.c_str());
    glUniformMatrix4fv(location, 1, GL_FALSE, &value[0][0]);
}

void Shader::setVec3(const std::string& name, const glm::vec3& value) const {
    const int location = glGetUniformLocation(m_program, name.c_str());
    glUniform3fv(location, 1, &value[0]);
}
