#pragma once
#include <array>
#include <numeric>
#include <random>

namespace mc::world {
    class PerlinNoise {
    public:
        explicit PerlinNoise(std::uint32_t seed = 0) {
            std::iota(p_.begin(), p_.end(), 0);
            std::ranges::shuffle(p_, std::default_random_engine{seed});
        }

        float noise(float x, float y) const {
            int X = static_cast<int>(floorf(x)) & 255;
            int Y = static_cast<int>(floorf(y)) & 255;
            x -= floorf(x);
            y -= floorf(y);

            float u = fade(x);
            float v = fade(y);

            int aa = p_[hash(X, Y)];
            int ba = p_[hash(X + 1, Y)];
            int ab = p_[hash(X, Y + 1)];
            int bb = p_[hash(X + 1, Y + 1)];

            float lerp1 = lerp(grad(aa, x, y),
                               grad(ba, x - 1, y), u);
            float lerp2 = lerp(grad(ab, x, y - 1),
                               grad(bb, x - 1, y - 1), u);
            return lerp(lerp1, lerp2, v); // [-1, 1]
        }

    private:
        std::array<int, 256> p_{};

        int hash(int x, int y) const { return (x + y) & 255; }
        float fade(float t) const { return t * t * (3.f - 2.f * t); }
        float lerp(float a, float b, float t) const { return a + t * (b - a); }

        static constexpr glm::vec2 GRAD[] = {
            {1, 2}, {-1, 2},
            {1, -2}, {-1, -2}
        };

        float grad(int h, float x, float y) const {
            const glm::vec2 &g = GRAD[h & 3];
            return g.x * x + g.y * y;
        }
    };
}
