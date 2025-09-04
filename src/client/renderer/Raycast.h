#pragma once
#include <optional>
#include <glm/glm.hpp>

#include "../../common/world/Chunk.h"
#include "../../common/world/World.h"

namespace mc::world {
    struct RayHit {
        glm::ivec3 block; // voxel that was hit (world space)
        glm::ivec3 normal; // outward face normal (−1|0|+1 per axis)
    };

    constexpr glm::ivec3 AXIS_UNIT[3] = {
        {1, 0, 0}, // X axis
        {0, 1, 0}, // Y axis
        {0, 0, 1} // Z axis
    };

    inline std::optional<RayHit> raycast(const World &world,
                                         const glm::vec3 &origin,
                                         const glm::vec3 &dir,
                                         float maxDist = 5.f) {
        glm::vec3 norm_dir = glm::normalize(dir);
        if (glm::length(norm_dir) < 1e-12f) return std::nullopt;

        glm::ivec3 cell = glm::floor(origin);
        glm::ivec3 step = glm::sign(norm_dir);

        glm::vec3 next = glm::vec3(cell) + glm::step(glm::vec3(0.f), norm_dir);
        glm::vec3 t_max = (next - origin) / norm_dir;
        glm::vec3 t_delta = glm::abs(glm::vec3(1.f) / norm_dir);

        float travelled = 0.f;
        int last_axis = -1;

        while (travelled <= maxDist) {
            ChunkLookup lookup = world.chunkLookup(cell);
            if (lookup.chunk && lookup.chunk->blockAt(lookup.local_coord).opaque()) {
                glm::ivec3 normal = last_axis == -1 ? -step : step * AXIS_UNIT[last_axis];
                return RayHit{cell, normal};
            }

            /* advance to next voxel ---------------------------------------------- */
            int axis = t_max.x < t_max.y
                           ? (t_max.x < t_max.z ? 0 : 2)
                           : t_max.y < t_max.z ? 1 : 2;

            cell[axis] += step[axis];
            travelled = t_max[axis];
            t_max[axis] += t_delta[axis];
            last_axis = axis;
        }
        return std::nullopt;
    }
}
