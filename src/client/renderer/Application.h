#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <memory>

#include "InputSystem.h"
#include "Renderer.h"
#include "Player.h"

namespace mc::client {
    class Application {
    public:
        explicit Application(int width = 1280, int height = 720,
                             const char *title = "minecraft-clone");

        ~Application();

        void run() const;

    private:
        GLFWwindow *window_{nullptr};
        int width_{}, height_{};

        std::unique_ptr<Player> player_;
        std::unique_ptr<gfx::Renderer> renderer_;
        std::unique_ptr<InputSystem> input_system_;

        void initWindow(const char *title);

        void initOpenGL();
    };
}
