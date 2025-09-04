#pragma once
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <world/Direction.h>

namespace mc::world {
    struct Vertex {
        glm::u8vec3 position; // 3 B
        std::uint8_t packed_normal; // 1 B
        glm::u16vec2 uv; // 4 B

        // std::numeric_limits<std::uint16_t>::max() == 65535
        Vertex(const glm::ivec3 &pos,
               Direction direction,
               const glm::vec2 &uv)
            : position(pos),
              packed_normal(packNormal(direction)),
              uv(uv.x * std::numeric_limits<std::uint16_t>::max(),
                 uv.y * std::numeric_limits<std::uint16_t>::max()) {
        }

        std::uint8_t packNormal(Direction direction) {
            constexpr std::uint8_t LUT[DIRECTIONS_COUNT] = {
                0b000001, 0b000010, 0b000100,
                0b001000, 0b010000, 0b100000
            };
            return LUT[directionToIndex(direction)];
        }
    };

    static_assert(sizeof(Vertex) == 8,
                  "mc::world::Vertex must stay a tightly-packed struct of 8 bytes");
}
