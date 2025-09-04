#pragma once
#include <glad/glad.h>
#include <vector>

namespace mc::gfx {
    class BufferPool {
    public:
        static BufferPool &instance() {
            static BufferPool instance;
            return instance;
        }

        ~BufferPool() {
            for (GLuint id: vbos_)
                glDeleteBuffers(1, &id);
            for (GLuint id: ebos_)
                glDeleteBuffers(1, &id);
        }

        GLuint acquire(GLenum targetBuffer) {
            std::vector<GLuint> &free = targetBuffer == GL_ARRAY_BUFFER ? vbos_ : ebos_;
            if (!free.empty()) {
                GLuint id = free.back();
                free.pop_back();
                return id;
            }
            GLuint id;
            glCreateBuffers(1, &id);
            return id;
        }

        void release(GLenum targetBuffer, GLuint id) {
            std::vector<GLuint> &free = targetBuffer == GL_ARRAY_BUFFER ? vbos_ : ebos_;
            free.emplace_back(id);
        }

        void clear() {
            for (GLuint id: vbos_)
                glDeleteBuffers(1, &id);
            for (GLuint id: ebos_)
                glDeleteBuffers(1, &id);
            vbos_.clear();
            ebos_.clear();
        }

    private:
        std::vector<GLuint> vbos_, ebos_;
    };
}
