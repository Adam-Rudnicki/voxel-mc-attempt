#pragma once
#include <array>
#include <memory>
#include <functional>
#include <glm/vec2.hpp>

#include "Chunk.h"
#include "Direction.h"
#include "WorldConstants.h"

namespace mc::world {
    class ChunkColumn {
    public:
        explicit ChunkColumn(const glm::ivec2 &coord) : coord_(coord) {
        }

        ~ChunkColumn() = default;

        const glm::ivec2 &coord() const { return coord_; }

        void linkNeighbor(Direction direction, ChunkColumn &other);

        void reset(const glm::ivec2 &newCoord);

        std::pair<Chunk *, const ChunkColumn *> adjacentChunkAndColumn(Direction direction, int index) const;

        std::array<Chunk *, DIRECTIONS_COUNT> adjacentChunks(int index) const;

        const auto &neighbors() const { return neighbors_; }

        auto &chunks() { return chunks_; }
        const auto &chunks() const { return chunks_; }

        void generateTerrain(const std::function<int(int, int)> &heightFn) {
            for (int i = 0; i < CHUNKS_PER_COLUMN; ++i) {
                glm::ivec3 chunk_coord(coord_.x, i, coord_.y);
                auto chunk = std::make_unique<Chunk>(chunk_coord);

                int slice_min_Y = chunk_coord.y * CHUNK_XYZ;
                int slice_max_Y = slice_min_Y + CHUNK_XYZ - 1;

                for (int z = 0; z < CHUNK_XYZ; ++z)
                    for (int x = 0; x < CHUNK_XYZ; ++x) {
                        int wx = coord_.x * CHUNK_XYZ + x;
                        int wz = coord_.y * CHUNK_XYZ + z;
                        int h = heightFn(wx, wz);

                        if (h < slice_min_Y) continue;

                        int top = std::min(h, slice_max_Y) - slice_min_Y;
                        for (int y = 0; y <= top; ++y) {
                            int wy = slice_min_Y + y;
                            BlockId id = wy == h ? BlockId::Grass : wy >= h - 3 ? BlockId::Dirt : BlockId::Stone;
                            chunk->setBlock({x, y, z}, id);
                        }

                        if (i == 0) chunk->setBlock({x, 0, z}, BlockId::Bedrock);
                    }

                if (chunk->isEmpty()) continue;
                chunks_[i] = std::move(chunk);
            }
        }

    private:
        glm::ivec2 coord_;
        std::array<std::unique_ptr<Chunk>, CHUNKS_PER_COLUMN> chunks_{};

        std::array<ChunkColumn *, HORIZONTAL_DIRECTIONS_COUNT> neighbors_{};
    };
}
