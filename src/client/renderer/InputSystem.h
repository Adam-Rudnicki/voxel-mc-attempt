#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Player.h"
#include "Renderer.h"

namespace mc::client {
    class InputSystem {
    public:
        InputSystem(GLFWwindow &window,
                    Player &player,
                    gfx::Renderer &renderer);

        void update(float dt) const;

    private:
        GLFWwindow &window_;
        Player &player_;
        gfx::Renderer &renderer_;

        double last_x_ = 0.0, last_y_ = 0.0;

        void handleCursorPosition(double x, double y);

        void handleScroll(double yOffset) const;

        bool key(int key) const { return glfwGetKey(&window_, key) == GLFW_PRESS; }
        bool mouse(int button) const { return glfwGetMouseButton(&window_, button) == GLFW_PRESS; }
    };
}
