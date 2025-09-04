#pragma once
#include <unordered_map>
#include <glm/glm.hpp>
#include <future>

#include "Chunk.h"
#include "ChunkColumn.h"
#include "PerlinNoise.h"

namespace mc::world {
    struct ColumnHash {
        std::size_t operator()(const glm::ivec2 &columnCoord) const noexcept {
            std::uint64_t h = static_cast<std::uint64_t>(columnCoord.x) * 73856093ull;
            h ^= static_cast<std::uint64_t>(columnCoord.y) * 83492791ull;
            return h;
        }
    };

    struct ChunkLookup {
        ChunkColumn *chunk_column;
        Chunk *chunk;
        int index;
        glm::ivec3 local_coord;
    };

    enum class TerrainGenerationMode {
        SineWave,
        PerlinNoise
    };

    class World {
    public:
        explicit World(TerrainGenerationMode terrainGenerationMode = TerrainGenerationMode::SineWave,
                       std::uint32_t seed = 0)
            : terrain_generation_mode_{terrainGenerationMode}, seed_{seed}, perlin_noise_{seed} {
            chunk_columns_.reserve(LOAD_AREA_SIZE);
            last_stream_centre_ = glm::ivec2(std::numeric_limits<int>::min());
        }

        const auto &chunk_columns() const { return chunk_columns_; }

        void setSeed(std::uint32_t seed) {
            seed_ = seed;
            perlin_noise_ = PerlinNoise(seed);
            chunk_columns_.clear();
            last_stream_centre_ = glm::ivec2(std::numeric_limits<int>::min());
        }

        void toggleTerrainMode() {
            terrain_generation_mode_ = terrain_generation_mode_ == TerrainGenerationMode::SineWave
                                           ? TerrainGenerationMode::PerlinNoise
                                           : TerrainGenerationMode::SineWave;
            chunk_columns_.clear();
            last_stream_centre_ = glm::ivec2(std::numeric_limits<int>::min());
        }

        glm::ivec2 worldToColumn(const glm::ivec3 &worldCoord) {
            return {
                worldCoord.x >> CHUNK_BITS,
                worldCoord.z >> CHUNK_BITS
            };
        }

        ChunkLookup chunkLookup(const glm::ivec3 &worldCoord) {
            glm::ivec2 column_coord = worldToColumn(worldCoord);

            if (auto it = chunk_columns_.find(column_coord); it != chunk_columns_.end()) {
                int index = worldToChunk(worldCoord).y;
                if (index < 0 || index >= CHUNKS_PER_COLUMN)
                    return {it->second.get(), nullptr, -1, {}};

                glm::ivec3 local_coord = worldCoord & CHUNK_MASK;
                return {it->second.get(), it->second->chunks()[index].get(), index, local_coord};
            }
            return {nullptr, nullptr, -1, {}};
        }

        ChunkLookup chunkLookup(const glm::ivec3 &worldCoord) const {
            return const_cast<World *>(this)->chunkLookup(worldCoord);
        }

        std::vector<ChunkColumn *> streamChunkColumns(const glm::ivec2 &centre) {
            if (centre == last_stream_centre_) return {};
            last_stream_centre_ = centre;

            std::erase_if(chunk_columns_, [&](const auto &kv) {
                glm::ivec2 d = kv.first - centre;
                return d.x * d.x + d.y * d.y > LOAD_RADIUS * LOAD_RADIUS;
            });

            std::vector<std::future<std::unique_ptr<ChunkColumn> > > futures;

            for (auto &offset: LOAD_RADIUS_OFFSETS) {
                glm::ivec2 column_coord = centre + offset;
                if (chunk_columns_.contains(column_coord)) continue;

                futures.emplace_back(std::async(
                    std::launch::async, [this, column_coord]() -> std::unique_ptr<ChunkColumn> {
                        auto column = std::make_unique<ChunkColumn>(column_coord);

                        if (terrain_generation_mode_ == TerrainGenerationMode::SineWave)
                            column->generateTerrain([this](int wx, int wz) { return sineHeight(wx, wz, seed_); });
                        else column->generateTerrain([this](int wx, int wz) { return perlinHeight(wx, wz); });

                        return std::move(column);
                    }));
            }

            std::vector<ChunkColumn *> created_columns;
            created_columns.reserve(futures.size());

            for (auto &f: futures) {
                auto column_ptr = f.get();
                ChunkColumn *column = column_ptr.get();
                chunk_columns_.emplace(column->coord(), std::move(column_ptr));
                created_columns.emplace_back(column);
            }

            for (ChunkColumn *column: created_columns)
                for (Direction direction: HORIZONTAL_DIRECTIONS) {
                    glm::ivec2 neighbor_column_coord = column->coord() + horizontalDirectionToNormalOffset(direction);
                    if (auto it = chunk_columns_.find(neighbor_column_coord); it != chunk_columns_.end())
                        column->linkNeighbor(direction, *it->second);
                }

            return created_columns;
        }

        TerrainGenerationMode terrain_generation_mode() const { return terrain_generation_mode_; }
        int seed() const { return seed_; }

    private:
        std::unordered_map<glm::ivec2, std::unique_ptr<ChunkColumn>, ColumnHash> chunk_columns_;

        TerrainGenerationMode terrain_generation_mode_;
        std::uint32_t seed_ = 0;
        PerlinNoise perlin_noise_;
        glm::ivec2 last_stream_centre_;

        glm::ivec3 worldToChunk(const glm::ivec3 &worldCoord) {
            return {
                worldCoord.x >> CHUNK_BITS,
                worldCoord.y >> CHUNK_BITS,
                worldCoord.z >> CHUNK_BITS
            };
        }

        int sineHeight(int wx, int wz,
                       int seed,
                       int base = 64,
                       int amplitude = 48) {
            constexpr float INV_64_TAU = 1.f / 64.f * 6.283185f;

            float sx = std::sinf((wx + seed) * INV_64_TAU);
            float cz = std::cosf((wz + seed) * INV_64_TAU);
            float h = base + amplitude * (sx + cz) / 2.f;
            return std::clamp(static_cast<int>(h), MIN_WORLD_Y, MAX_WORLD_Y);
        }

        int perlinHeight(int wx, int wz,
                         int base = 80,
                         int amplitude = 80,
                         float scale = 32.f) const {
            float h_f = perlin_noise_.noise(static_cast<float>(wx) / scale,
                                            static_cast<float>(wz) / scale);
            int h = base + static_cast<int>(h_f * amplitude);
            return std::clamp(h, MIN_WORLD_Y, MAX_WORLD_Y);
        }
    };
}
