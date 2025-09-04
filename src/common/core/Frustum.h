#pragma once
#include <array>

namespace mc::core {
    struct Plane {
        glm::vec3 n;
        float d;
    };

    class Frustum {
    public:
        explicit Frustum(const glm::mat4 &mvp) { extractPlanes(mvp); }

        bool intersectsAABB(const glm::ivec3 &boxMin, const glm::ivec3 &boxMax) const {
            auto f_box_min = glm::vec3(boxMin);
            auto f_box_max = glm::vec3(boxMax);
            return std::ranges::all_of(planes_, [&](const Plane &plane) {
                glm::vec3 vertex{
                    (plane.n.x >= 0 ? f_box_max.x : f_box_min.x),
                    (plane.n.y >= 0 ? f_box_max.y : f_box_min.y),
                    (plane.n.z >= 0 ? f_box_max.z : f_box_min.z)
                };
                return glm::dot(plane.n, vertex) + plane.d >= 0;
            });
        }

    private:
        std::array<Plane, 6> planes_{};

        void extractPlanes(const glm::mat4 &mvp) {
            auto row = [&](int i) { return glm::vec4(mvp[0][i], mvp[1][i], mvp[2][i], mvp[3][i]); };

            glm::vec4 r0 = row(0), r1 = row(1), r2 = row(2), r3 = row(3);

            glm::vec4 raw_planes[6] = {
                r3 + r0, // left
                r3 - r0, // right
                r3 + r1, // bottom
                r3 - r1, // top
                r3 + r2, // near
                r3 - r2 // far
            };

            for (int i = 0; i < 6; ++i) {
                auto n = glm::vec3(raw_planes[i]);
                float w = raw_planes[i].w;
                float length = glm::length(n);
                planes_[i] = {n / length, w / length}; // normalize
            }
        }
    };
}
