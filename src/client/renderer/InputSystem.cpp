#include "InputSystem.h"
#include "Raycast.h"

using namespace mc::client;

InputSystem::InputSystem(GLFWwindow &window,
                         Player &player,
                         gfx::Renderer &renderer)
    : window_{window}, player_{player}, renderer_{renderer} {
    glfwSetWindowUserPointer(&window_, this);
    glfwSetInputMode(&window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(&window_, [](GLFWwindow *window, double x, double y) {
        if (auto *self = static_cast<InputSystem *>(glfwGetWindowUserPointer(window)))
            self->handleCursorPosition(x, y);
    });
    glfwSetScrollCallback(&window_, [](GLFWwindow *window, double xOffset, double yOffset) {
        if (auto *self = static_cast<InputSystem *>(glfwGetWindowUserPointer(window)))
            self->handleScroll(yOffset);
    });
}

void InputSystem::update(float dt) const {
    player_.camera().handleKeyboard(key(GLFW_KEY_W), key(GLFW_KEY_S),
                                    key(GLFW_KEY_A), key(GLFW_KEY_D),
                                    key(GLFW_KEY_SPACE), key(GLFW_KEY_LEFT_SHIFT), dt);

    for (int i = 0; i < 9; ++i)
        if (key(GLFW_KEY_1 + i)) player_.selectSlot(i);

    static bool r_prev = false, p_prev = false;
    static int seed = 1;

    bool r_now = key(GLFW_KEY_R);
    if (r_now && !r_prev) renderer_.regenerateTerrain(seed++);
    r_prev = r_now;

    bool p_now = key(GLFW_KEY_P);
    if (p_now && !p_prev) renderer_.toggleTerrainGenerationMode();
    p_prev = p_now;

    static bool l_prev_mb = false, r_prev_mb = false;
    bool l_now_mb = mouse(GLFW_MOUSE_BUTTON_LEFT);
    bool r_now_mb = mouse(GLFW_MOUSE_BUTTON_RIGHT);

    std::optional<world::RayHit> hit = world::raycast(renderer_.world(), player_.camera().position(),
                                                      player_.camera().front());
    renderer_.setHighlightBlock(hit ? std::optional{hit->block} : std::nullopt);

    bool l_click = l_now_mb && !l_prev_mb;
    bool r_click = r_now_mb && !r_prev_mb;

    if (l_click || r_click) {
        if (hit) {
            if (l_click) renderer_.breakBlock(hit->block);
            else renderer_.placeBlock(hit->block - hit->normal, player_.currentBlock());
        }
    }

    l_prev_mb = l_now_mb;
    r_prev_mb = r_now_mb;
}

void InputSystem::handleCursorPosition(double x, double y) {
    double dx = last_x_ - x;
    double dy = last_y_ - y;
    last_x_ = x;
    last_y_ = y;
    player_.camera().handleMouse(static_cast<float>(dx), static_cast<float>(dy));
}

void InputSystem::handleScroll(double yOffset) const {
    if (yOffset < 0) player_.selectNextSlot();
    if (yOffset > 0) player_.selectPrevSlot();
}
