#include <algorithm>

#include "Camera.h"

using namespace mc::core;

Camera::Camera(float fovDegree, float aspect,
               float nearPlane, float farPlane)
    : fov_{fovDegree}, aspect_{aspect},
      near_plane_{nearPlane}, far_plane_{farPlane} {
    updateVectors();
}

void Camera::handleMouse(float dx, float dy) {
    if (dx == 0.0f && dy == 0.0f) return;
    yaw_ += dx * sensitivity_;
    pitch_ += dy * sensitivity_;
    pitch_ = std::clamp(pitch_, -89.0f, 89.0f);
    updateVectors();
}

void Camera::handleKeyboard(bool forward, bool backward,
                             bool left, bool right,
                             bool space, bool shift,
                             float dt) {
    float velocity = speed_ * dt;
    if (forward) position_ += front_ * velocity;
    if (backward) position_ -= front_ * velocity;
    if (left) position_ -= right_ * velocity;
    if (right) position_ += right_ * velocity;
    if (space) position_ += world_up_ * velocity; // move up
    if (shift) position_ -= world_up_ * velocity; // move down
}

glm::mat4 Camera::view() const {
    return glm::lookAtLH(position_, position_ + front_, up_);
}

glm::mat4 Camera::projection() const {
    return glm::perspectiveLH(glm::radians(fov_), aspect_, near_plane_, far_plane_);
}

glm::mat4 Camera::viewProjection() const {
    return projection() * view();
}

void Camera::updateVectors() {
    float cy = cos(glm::radians(yaw_));
    float sy = sin(glm::radians(yaw_));
    float cp = cos(glm::radians(pitch_));
    float sp = sin(glm::radians(pitch_));

    front_ = glm::normalize(glm::vec3{
        cy * cp,
        sp,
        sy * cp
    });
    right_ = glm::normalize(glm::cross(world_up_, front_));
    up_ = glm::normalize(glm::cross(front_, right_));
}
