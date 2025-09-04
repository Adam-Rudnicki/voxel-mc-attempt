#pragma once
#include <string>
#include <glad/glad.h>

namespace mc::gfx {
    class TextureAtlas {
    public:
        explicit TextureAtlas(const std::string &filePath, int tilePx = 16);

        ~TextureAtlas();

        TextureAtlas(const TextureAtlas &) = delete;

        TextureAtlas &operator=(const TextureAtlas &) = delete;

        void bind(int unit = 0) const;

        int tiles_per_row() const { return tiles_per_row_; }

    private:
        GLuint textures_{0};
        int tiles_per_row_{1};
        float step_{1.0f};
    };
}
