#pragma once
#include <array>
#include <cassert>
#include <glm/vec3.hpp>

#include "Block.h"
#include "Direction.h"
#include "WorldConstants.h"

namespace mc::world {
    constexpr int CHUNK_VOLUME = CHUNK_XYZ * CHUNK_XYZ * CHUNK_XYZ;
    constexpr auto CHUNK_SIZE_VEC = glm::ivec3(CHUNK_XYZ);
    constexpr int CHUNK_SLICE_VOLUME = CHUNK_XYZ * CHUNK_XYZ; // 256
    constexpr int LAST = CHUNK_XYZ - 1; // 15

    struct ChunkHash {
        std::size_t operator()(const glm::ivec3 &chunkCoord) const noexcept {
            std::uint64_t h = static_cast<std::uint64_t>(chunkCoord.x) * 73856093ull;
            h ^= static_cast<std::uint64_t>(chunkCoord.y) * 19349669ull;
            h ^= static_cast<std::uint64_t>(chunkCoord.z) * 83492791ull;
            return h;
        }
    };

    using SideFace = std::array<Block, CHUNK_XYZ * CHUNK_XYZ>;
    using NeighborSideFaces = std::array<SideFace, DIRECTIONS_COUNT>;

    class Chunk {
    public:
        explicit Chunk(const glm::ivec3 &coord) : coord_(coord) {
        }

        ~Chunk() = default;

        const Block &blockAt(const glm::ivec3 &localCoord) const {
            assert(inBounds(localCoord));
            return blocks_[index(localCoord)];
        }

        void setBlock(const glm::ivec3 &localCoord, BlockId blockId) {
            assert(inBounds(localCoord));
            Block &cell = blocks_[index(localCoord)];
            if (cell.id == blockId) return; // no change

            if (cell.opaque()) --non_air_blocks_;
            cell = Block{blockId};
            if (cell.opaque()) ++non_air_blocks_;
        }

        bool isEmpty() const { return non_air_blocks_ == 0; }

        static bool inBounds(const glm::ivec3 &localCoord) {
            auto ux = static_cast<std::uint32_t>(localCoord.x);
            auto uy = static_cast<std::uint32_t>(localCoord.y);
            auto uz = static_cast<std::uint32_t>(localCoord.z);
            return (ux | uy | uz) < static_cast<std::uint32_t>(CHUNK_XYZ);
        }

        const glm::ivec3 &coord() const { return coord_; }

        NeighborSideFaces collectNeighborSideFaces(const std::array<Chunk *, DIRECTIONS_COUNT> &neighbors) const {
            NeighborSideFaces faces{};

            for (Direction direction: DIRECTIONS) {
                uint8_t index = directionToIndex(direction);
                if (Chunk *neighbor = neighbors[index])
                    faces[index] = neighbor->retrieveSideFace(oppositeDirection(direction));
            }
            return faces;
        }

        const Block &getNeighborBlock(const NeighborSideFaces &neighborSideFaces,
                                      const glm::ivec3 &localCoord,
                                      Direction direction) const {
            const SideFace &plane = neighborSideFaces[directionToIndex(direction)];

            switch (direction) {
                case Direction::PositiveX:
                case Direction::NegativeX: return plane[localCoord.y * CHUNK_XYZ + localCoord.z];
                case Direction::PositiveY:
                case Direction::NegativeY: return plane[localCoord.z * CHUNK_XYZ + localCoord.x];
                case Direction::PositiveZ:
                case Direction::NegativeZ: return plane[localCoord.y * CHUNK_XYZ + localCoord.x];
            }
        }

    private:
        std::array<Block, CHUNK_VOLUME> blocks_{};
        glm::ivec3 coord_;
        int non_air_blocks_ = 0;

        std::size_t index(const glm::ivec3 &localCoord) const {
            return (localCoord.y * CHUNK_XYZ + localCoord.z) * CHUNK_XYZ + localCoord.x;
        }

        SideFace retrieveSideFace(Direction direction) const {
            SideFace out{};

            switch (direction) {
                case Direction::PositiveY: {
                    std::size_t base = LAST * CHUNK_SLICE_VOLUME;
                    std::copy_n(blocks_.data() + base, CHUNK_SLICE_VOLUME, out.begin());
                    break;
                }
                case Direction::NegativeY: {
                    std::copy_n(blocks_.data(), CHUNK_SLICE_VOLUME, out.begin());
                    break;
                }

                case Direction::PositiveZ:
                case Direction::NegativeZ: {
                    int z = direction == Direction::PositiveZ ? LAST : 0;
                    for (int y = 0; y < CHUNK_XYZ; ++y) {
                        const Block *src = blocks_.data() + (y * CHUNK_XYZ + z) * CHUNK_XYZ;
                        std::copy_n(src, CHUNK_XYZ, out.begin() + y * CHUNK_XYZ);
                    }
                    break;
                }

                case Direction::PositiveX:
                case Direction::NegativeX: {
                    int x = direction == Direction::PositiveX ? LAST : 0;
                    const Block *base = blocks_.data() + x;
                    for (int y = 0; y < CHUNK_XYZ; ++y) {
                        const Block *row = base + y * CHUNK_SLICE_VOLUME;
                        for (int z = 0; z < CHUNK_XYZ; ++z)
                            out[y * CHUNK_XYZ + z] = row[z * CHUNK_XYZ];
                    }
                    break;
                }
            }

            return out;
        }
    };
}
