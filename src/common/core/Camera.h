#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace mc::core {
    class Camera {
    public:
        Camera(float fovDegree, float aspect,
               float nearPlane = 0.1f, float farPlane = 1000.f);

        void handleMouse(float dx, float dy);

        void handleKeyboard(bool forward, bool backward,
                             bool left, bool right,
                             bool space, bool shift,
                             float dt);

        void setViewport(float aspect) { aspect_ = aspect; }

        glm::mat4 view() const;

        glm::mat4 projection() const;

        glm::mat4 viewProjection() const;

        const glm::vec3 &position() const { return position_; }

        const glm::vec3 &front() const { return front_; }

    private:
        void updateVectors();

        glm::vec3 position_{0.0f, 30.0f, 0.0f};
        glm::vec3 front_{0.0f, 0.0f, 1.0f};
        glm::vec3 up_{0.0f, 1.0f, 0.0f};
        glm::vec3 right_{1.0f, 0.0f, 0.0f};
        static constexpr glm::vec3 world_up_{0.0f, 1.0f, 0.0f};

        float yaw_ = 90.0f;
        float pitch_ = 0.0f;
        float speed_ = 10.0f;
        float sensitivity_ = 0.1f;
        float fov_ = 60.0f;
        float aspect_ = 16.0f / 9.0f;
        float near_plane_ = 0.1f;
        float far_plane_ = 1000.0f;
    };
}
