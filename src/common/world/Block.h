#pragma once
#include "Direction.h"

namespace mc::world {
    constexpr std::size_t NUM_BLOCKS = 7;

    enum class BlockId : std::uint8_t {
        Air = 0,
        Grass = 1,
        Dirt = 2,
        Stone = 3,
        Bedrock = 4,
        Wood = 5,
        Leaves = 6,
    };

    constexpr std::array<bool, NUM_BLOCKS> OPAQUE_LUT = [] {
        std::array<bool, NUM_BLOCKS> lut{};
        for (std::size_t i = 0; i < lut.size(); ++i)
            lut[i] = static_cast<BlockId>(i) != BlockId::Air;
        return lut;
    }();

    constexpr std::array<bool, NUM_BLOCKS> OCCLUDING_LUT = [] {
        std::array<bool, NUM_BLOCKS> lut{};
        for (std::size_t i = 0; i < lut.size(); ++i) {
            auto id = static_cast<BlockId>(i);
            lut[i] = id != BlockId::Air && id != BlockId::Leaves;
        }
        return lut;
    }();

    constexpr std::array<bool, NUM_BLOCKS> LEAVES_LUT = [] {
        std::array<bool, NUM_BLOCKS> lut{};
        for (std::size_t i = 0; i < lut.size(); ++i)
            lut[i] = static_cast<BlockId>(i) == BlockId::Leaves;
        return lut;
    }();

    enum class RenderLayer : std::uint8_t { Occluding = 0, Cutout = 1 };

    constexpr std::array<RenderLayer, NUM_BLOCKS> RENDER_LAYER_LUT = [] {
        std::array<RenderLayer, NUM_BLOCKS> lut{};
        for (std::size_t i = 0; i < lut.size(); ++i)
            lut[i] = static_cast<BlockId>(i) == BlockId::Leaves ? RenderLayer::Cutout : RenderLayer::Occluding;
        return lut;
    }();

    // regarding tile atlas: face: 0: +X, 1: -X, 2: +Y, 3: -Y, 4: +Z, 5: -Z
    static constexpr std::uint8_t TILE_LUT[NUM_BLOCKS][DIRECTIONS_COUNT] = {
        {0, 0, 0, 0, 0, 0}, // Air
        {1, 1, 0, 1, 1, 1}, // Grass
        {1, 1, 1, 1, 1, 1}, // Dirt
        {2, 2, 2, 2, 2, 2}, // Stone
        {3, 3, 3, 3, 3, 3}, // Bedrock
        {4, 4, 4, 4, 4, 4}, // Wood
        {5, 5, 5, 5, 5, 5} // Leaves
    };

    struct Block {
        BlockId id{BlockId::Air};

        bool opaque() const { return OPAQUE_LUT[static_cast<std::uint8_t>(id)]; }

        bool occluding() const { return OCCLUDING_LUT[static_cast<std::uint8_t>(id)]; }

        bool isLeaves() const { return LEAVES_LUT[static_cast<std::uint8_t>(id)]; }

        RenderLayer renderLayer() const { return RENDER_LAYER_LUT[static_cast<std::uint8_t>(id)]; }

        int tile(Direction direction) const { return TILE_LUT[static_cast<std::uint8_t>(id)][directionToIndex(direction)]; }
    };
}
