#pragma once
#include <string>
#include <glad/glad.h>

namespace mc::gfx {
    class Shader {
    public:
        Shader(const std::string &vertPath,
               const std::string &fragPath);

        ~Shader();

        Shader(const Shader &) = delete;

        Shader &operator=(const Shader &) = delete;

        Shader(Shader &&) noexcept;

        Shader &operator=(Shader &&) noexcept;

        void use() const {
            glUseProgram(program_);
        }

        GLuint id() const { return program_; }

    private:
        GLuint program_ = 0;

        GLuint compileStage(const std::string &src, GLenum type);

        std::string loadFile(const std::string &path);
    };
}
