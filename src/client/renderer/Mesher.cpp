#include "Mesher.h"

using namespace mc::world;

// quad vertex offsets per face (4)
constexpr glm::ivec3 QUAD[DIRECTIONS_COUNT][4] = {
    // +X
    {{1, 0, 0}, {1, 1, 0}, {1, 1, 1}, {1, 0, 1}},
    // -X
    {{0, 0, 1}, {0, 1, 1}, {0, 1, 0}, {0, 0, 0}},
    // +Y
    {{0, 1, 0}, {0, 1, 1}, {1, 1, 1}, {1, 1, 0}},
    // -Y
    {{0, 0, 1}, {0, 0, 0}, {1, 0, 0}, {1, 0, 1}},
    // +Z
    {{1, 0, 1}, {1, 1, 1}, {0, 1, 1}, {0, 0, 1}},
    // -Z
    {{0, 0, 0}, {0, 1, 0}, {1, 1, 0}, {1, 0, 0}}
};

constexpr std::uint32_t Q[DIRECTIONS_COUNT] = {0, 1, 2, 0, 2, 3};

namespace {
    std::array<glm::vec2, 4> quadUV(int tile, int tilesPerRow) {
        float step = 1.f / static_cast<float>(tilesPerRow);
        float x = (tile % tilesPerRow) * step;
        float y = (tile / tilesPerRow) * step;
        return {
            glm::vec2{x, y},
            glm::vec2{x, y + step},
            glm::vec2{x + step, y + step},
            glm::vec2{x + step, y}
        };
    }

    // even direction indices are canonical (0, 2, 4) for +X, +Y, +Z
    bool isCanonicalDirection(Direction direction) { return (directionToIndex(direction) & 1) == 0; }
}

MeshLayers Mesher::buildChunkMeshLayers(const Chunk &chunk,
                                        const std::array<Chunk *, DIRECTIONS_COUNT> &neighbors,
                                        gfx::TextureAtlas &textureAtlas) {
    MeshLayers out;
    out.vertices[0].reserve(CHUNK_SLICE_VOLUME * 4 * 3);
    out.indices[0].reserve(CHUNK_SLICE_VOLUME * 6 * 3);

    NeighborSideFaces neighbor_faces = chunk.collectNeighborSideFaces(neighbors);

    for (int y = 0; y < CHUNK_XYZ; ++y)
        for (int z = 0; z < CHUNK_XYZ; ++z)
            for (int x = 0; x < CHUNK_XYZ; ++x) {
                glm::ivec3 local_coord{x, y, z};
                const Block &block = chunk.blockAt(local_coord);
                if (!block.opaque()) continue;

                int bucket = block.renderLayer() == RenderLayer::Occluding ? 0 : 1;
                bool is_leaves = block.isLeaves();

                for (Direction direction: DIRECTIONS) {
                    glm::ivec3 adjacent_local_coord = local_coord + directionToNormalOffset(direction);

                    const Block &adjacent_block =
                            Chunk::inBounds(adjacent_local_coord)
                                ? chunk.blockAt(adjacent_local_coord)
                                : chunk.getNeighborBlock(neighbor_faces, adjacent_local_coord, direction);

                    bool both_leaves = is_leaves && adjacent_block.isLeaves();
                    bool skip_face = both_leaves
                                         ? isCanonicalDirection(direction)
                                         : adjacent_block.occluding();

                    if (skip_face) continue;

                    int tile = block.tile(direction);
                    std::array<glm::vec2, 4> UV = quadUV(tile, textureAtlas.tiles_per_row());
                    auto base = static_cast<std::uint32_t>(out.vertices[bucket].size());

                    std::vector<Vertex> &vertices = out.vertices[bucket];
                    for (int v = 0; v < 4; ++v) {
                        const glm::ivec3 &offset = QUAD[directionToIndex(direction)][v];
                        vertices.emplace_back(
                            local_coord + offset,
                            direction,
                            UV[v]
                        );
                    }

                    // two triangles 0-1-2 and 0-2-3
                    std::vector<std::uint32_t> &indices = out.indices[bucket];
                    for (uint32_t q: Q) indices.emplace_back(base + q);
                }
            }

    return out;
}
