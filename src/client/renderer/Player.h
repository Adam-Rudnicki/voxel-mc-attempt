#pragma once
#include <array>

#include "../../common/core/Camera.h"
#include "../../common/world/Block.h"

namespace mc::client {
    class Player {
    public:
        explicit Player(float fovDegree = 70.f,
                        float aspect = 16.f / 9.f,
                        float nearPlane = 0.1f,
                        float farPlane = 1'000.f);

        core::Camera &camera() { return camera_; }
        const core::Camera &camera() const { return camera_; }

        void selectNextSlot();

        void selectPrevSlot();

        void selectSlot(int index);

        int currentSlot() const { return current_slot_; }
        world::BlockId currentBlock() const { return hotbar_[current_slot_]; }

    private:
        core::Camera camera_;

        std::array<world::BlockId, 6> hotbar_{
            world::BlockId::Grass,
            world::BlockId::Dirt,
            world::BlockId::Stone,
            world::BlockId::Bedrock,
            world::BlockId::Wood,
            world::BlockId::Leaves,
        };
        int current_slot_ = 0;
    };
}
