#pragma once
#include <array>
#include <glm/vec2.hpp>

namespace mc::world {
    constexpr int CHUNK_BITS = 5; // 5 bits for 32 blocks per axis
    constexpr int CHUNK_XYZ = 1 << CHUNK_BITS; // 32
    constexpr int CHUNK_MASK = CHUNK_XYZ - 1; // 31

    constexpr int CHUNKS_PER_COLUMN = 8;
    constexpr int WORLD_HEIGHT = CHUNK_XYZ * CHUNKS_PER_COLUMN; // 256
    constexpr int MIN_WORLD_Y = 0;
    constexpr int MAX_WORLD_Y = WORLD_HEIGHT - 1;

    constexpr int RENDER_RADIUS = 32;
    constexpr int LOAD_RADIUS = RENDER_RADIUS + 1;

    constexpr int numberOfElementsInEuclideanRadius(int radius) {
        int count = 0;
        for (int dx = -radius; dx <= radius; ++dx)
            for (int dz = -radius; dz <= radius; ++dz)
                if (dx * dx + dz * dz <= radius * radius) ++count;
        return count;
    }

    constexpr int RENDER_AREA_SIZE = numberOfElementsInEuclideanRadius(RENDER_RADIUS);
    constexpr int LOAD_AREA_SIZE = numberOfElementsInEuclideanRadius(LOAD_RADIUS);

    template<int RADIUS, int SIZE>
    constexpr std::array<glm::ivec2, SIZE> makeOffsets() {
        std::array<glm::ivec2, SIZE> offsets{};
        int index = 0;
        for (int dx = -RADIUS; dx <= RADIUS; ++dx)
            for (int dz = -RADIUS; dz <= RADIUS; ++dz)
                if (dx * dx + dz * dz <= RADIUS * RADIUS)
                    offsets[index++] = {dx, dz};
        return offsets;
    }

    constexpr auto RENDER_RADIUS_OFFSETS = makeOffsets<RENDER_RADIUS, RENDER_AREA_SIZE>();
    constexpr auto LOAD_RADIUS_OFFSETS = makeOffsets<LOAD_RADIUS, LOAD_AREA_SIZE>();
}
