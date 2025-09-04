#include <spdlog/spdlog.h>

#include "Application.h"
#include "BufferPool.h"

using namespace mc::client;

Application::Application(int width, int height, const char *title)
    : width_{width}, height_{height} {
    initWindow(title);
    initOpenGL();
    float aspect = static_cast<float>(width) / height;
    player_ = std::make_unique<Player>(70.f, aspect);
    renderer_ = std::make_unique<gfx::Renderer>();
    input_system_ = std::make_unique<InputSystem>(*window_, *player_, *renderer_);
}

Application::~Application() {
    gfx::BufferPool::instance().clear();
    glfwTerminate();
}

void Application::run() const {
    glClearColor(0.73f, 0.80f, 0.85f, 1.0f);

    double prev = glfwGetTime();
    while (!glfwWindowShouldClose(window_)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        double now = glfwGetTime();
        float dt = static_cast<float>(now - prev);
        prev = now;

        input_system_->update(dt);
        renderer_->streamMeshColumns(player_->camera());
        renderer_->renderFrame(player_->camera());

        glfwSwapBuffers(window_);
        glfwPollEvents();
    }
}

void Application::initWindow(const char *title) {
    if (!glfwInit()) throw std::runtime_error("GLFW init failed");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);

    window_ = glfwCreateWindow(width_, height_, title, nullptr, nullptr);
    if (!window_) throw std::runtime_error("Window creation failed");

    glfwMakeContextCurrent(window_);
    glfwSwapInterval(0); // vsync
}

void Application::initOpenGL() {
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
        throw std::runtime_error("Failed to load OpenGL functions with GLAD");

    spdlog::info("OpenGL {}.{} loaded", GLVersion.major, GLVersion.minor);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CW);
}
