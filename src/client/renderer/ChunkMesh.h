#pragma once
#include <glad/glad.h>

#include "Mesher.h"

namespace mc::gfx {
    class ChunkMesh {
    public:
        ChunkMesh(const world::Chunk &chunk,
                  const std::array<world::Chunk *, world::DIRECTIONS_COUNT> &neighbors,
                  TextureAtlas &textureAtlas);

        ~ChunkMesh();

        void buildLayers();

        void rebuild(const world::Chunk &chunk,
                     const std::array<world::Chunk *, world::DIRECTIONS_COUNT> &neighbors,
                     TextureAtlas &textureAtlas);

        void drawOccluding() const;

        void drawCutout() const;

        bool isEmpty() const { return index_count_[0] == 0 && index_count_[1] == 0; }

        const glm::ivec3 &origin() const { return aabb_min_; }
        const glm::ivec3 &aabb_min() const { return aabb_min_; }
        const glm::ivec3 &aabb_max() const { return aabb_max_; }
        const glm::ivec3 &aabb_center() const { return aabb_center_; }

    private:
        static constexpr int NUM_RENDER_LAYERS = 2; // 0-occluding, 1-cutout

        world::MeshLayers layers;

        GLuint vao_[NUM_RENDER_LAYERS]{};
        GLuint vbo_[NUM_RENDER_LAYERS]{};
        GLuint ebo_[NUM_RENDER_LAYERS]{};
        std::size_t vbo_capacity_[NUM_RENDER_LAYERS]{};
        std::size_t ebo_capacity_[NUM_RENDER_LAYERS]{};
        std::uint32_t index_count_[NUM_RENDER_LAYERS]{};

        glm::ivec3 aabb_min_{}, aabb_max_{};
        glm::ivec3 aabb_center_{};

        void buildLayer(int layer);

        void drawInternal(int layer) const;

        void releaseLayer(int layer) const;
    };
}
