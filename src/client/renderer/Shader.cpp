#include <fstream>
#include <sstream>
#include <spdlog/spdlog.h>

#include "Shader.h"

using namespace mc::gfx;

Shader::Shader(const std::string &vertPath,
               const std::string &fragPath) {
    const std::string vert_src = loadFile(vertPath);
    const std::string frag_src = loadFile(fragPath);

    try {
        GLuint vs = compileStage(vert_src, GL_VERTEX_SHADER);
        GLuint fs = compileStage(frag_src, GL_FRAGMENT_SHADER);

        program_ = glCreateProgram();
        glAttachShader(program_, vs);
        glAttachShader(program_, fs);
        glLinkProgram(program_);

        GLint ok;
        glGetProgramiv(program_, GL_LINK_STATUS, &ok);
        if (!ok) {
            char log[1024];
            glGetProgramInfoLog(program_, 1024, nullptr, log);
            throw std::runtime_error(std::string("Shader link error:\n") + log);
        }
        glDeleteShader(vs);
        glDeleteShader(fs);
    } catch (const std::exception &e) {
        glDeleteProgram(program_);
        throw;
    }
}

Shader::~Shader() {
    glDeleteProgram(program_);
}

Shader::Shader(Shader &&other) noexcept {
    program_ = other.program_;
    other.program_ = 0;
}

Shader &Shader::operator=(Shader &&other) noexcept {
    if (this != &other) {
        glDeleteProgram(program_);
        program_ = other.program_;
        other.program_ = 0;
    }
    return *this;
}

GLuint Shader::compileStage(const std::string &src, GLenum type) {
    GLuint id = glCreateShader(type);
    const char *csrc = src.c_str();
    glShaderSource(id, 1, &csrc, nullptr);
    glCompileShader(id);
    GLint ok;
    glGetShaderiv(id, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[1024];
        glGetShaderInfoLog(id, 1024, nullptr, log);
        glDeleteShader(id);
        throw std::runtime_error(fmt::format("Shader compile error ({}):\n{}", type, log));
    }
    return id;
}

std::string Shader::loadFile(const std::string &path) {
    std::ifstream f(path, std::ios::binary);
    if (!f) throw std::runtime_error(fmt::format("Failed to open file: {}", path));
    std::stringstream ss;
    ss << f.rdbuf();
    return ss.str();
}
