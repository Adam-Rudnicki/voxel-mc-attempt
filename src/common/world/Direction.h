#pragma once
#include <array>
#include <cstdint>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace mc::world {
    enum class Direction : std::uint8_t {
        PositiveX = 0, NegativeX = 1,
        PositiveY = 2, NegativeY = 3,
        PositiveZ = 4, NegativeZ = 5
    };

    constexpr std::uint8_t DIRECTIONS_COUNT = 6;
    constexpr std::uint8_t HORIZONTAL_DIRECTIONS_COUNT = 4;

    constexpr std::uint8_t directionToIndex(Direction direction) {
        return static_cast<std::uint8_t>(direction);
    }

    constexpr std::uint8_t horizontalDirectionToIndex(Direction direction) {
        std::uint8_t index = directionToIndex(direction);
        return (index & 1u) | ((index >> 1) & 2u); // 0,1,2,3
    }

    constexpr std::uint8_t oppositeDirectionIndex(Direction direction) {
        return static_cast<std::uint8_t>(direction) ^ 1u;
    }

    constexpr Direction oppositeDirection(Direction direction) {
        return static_cast<Direction>(static_cast<std::uint8_t>(direction) ^ 1u);
    }

    constexpr std::array DIRECTIONS = {
        Direction::PositiveX, Direction::NegativeX,
        Direction::PositiveY, Direction::NegativeY,
        Direction::PositiveZ, Direction::NegativeZ
    };

    constexpr std::array HORIZONTAL_DIRECTIONS = {
        Direction::PositiveX, Direction::NegativeX,
        Direction::PositiveZ, Direction::NegativeZ
    };

    constexpr glm::ivec3 DIRECTION_NORMAL_OFFSETS[] = {
        {1, 0, 0}, {-1, 0, 0},
        {0, 1, 0}, {0, -1, 0},
        {0, 0, 1}, {0, 0, -1}
    };

    constexpr glm::ivec2 HORIZONTAL_DIRECTION_NORMAL_OFFSETS[] = {
        {1, 0}, {-1, 0},
        {0, 1}, {0, -1}
    };

    constexpr glm::ivec3 directionToNormalOffset(Direction direction) {
        return DIRECTION_NORMAL_OFFSETS[directionToIndex(direction)];
    }

    constexpr glm::ivec2 horizontalDirectionToNormalOffset(Direction direction) {
        return HORIZONTAL_DIRECTION_NORMAL_OFFSETS[horizontalDirectionToIndex(direction)];
    }
}
