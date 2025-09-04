#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <spdlog/spdlog.h>

#include "TextureAtlas.h"

using namespace mc::gfx;

TextureAtlas::TextureAtlas(const std::string &path, int tilePx) {
    int width, height, channels;
    unsigned char *data = stbi_load(path.c_str(), &width, &height, &channels, STBI_rgb_alpha);
    if (!data) throw std::runtime_error("Failed to load " + path);

    tiles_per_row_ = width / tilePx;
    step_ = 1.f / static_cast<float>(tiles_per_row_);

    glCreateTextures(GL_TEXTURE_2D, 1, &textures_);

    glTextureParameteri(textures_, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(textures_, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTextureParameteri(textures_, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(textures_, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTextureParameteri(textures_, GL_TEXTURE_BASE_LEVEL, 0);
    glTextureParameteri(textures_, GL_TEXTURE_MAX_LEVEL, 0);

    glTextureStorage2D(textures_, 1, GL_RGBA8, width, height);
    glTextureSubImage2D(textures_, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);

    stbi_image_free(data);
}

TextureAtlas::~TextureAtlas() {
    if (textures_)
        glDeleteTextures(1, &textures_);
}

void TextureAtlas::bind(int unit) const {
    glBindTextureUnit(unit, textures_);
}
