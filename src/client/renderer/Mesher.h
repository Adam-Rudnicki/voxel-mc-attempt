#pragma once
#include <vector>

#include "TextureAtlas.h"
#include "Vertex.h"
#include "../../common/world/Chunk.h"
#include "../../common/world/World.h"

namespace mc::world {
    struct MeshLayers {
        std::array<std::vector<Vertex>, 2> vertices; // 0-occluding, 1-cutout
        std::array<std::vector<std::uint32_t>, 2> indices;
    };

    class Mesher {
    public:
        static MeshLayers buildChunkMeshLayers(const Chunk &chunk,
                                               const std::array<Chunk *, DIRECTIONS_COUNT> &neighbors,
                                               gfx::TextureAtlas &textureAtlas);
    };
}
