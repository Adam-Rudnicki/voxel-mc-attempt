#include <spdlog/spdlog.h>

#include "renderer/Application.h"

int main() {
    try {
        mc::client::Application app;
        app.run();
        return 0;
    } catch (const std::exception &e) {
        spdlog::error("Fatal: {}", e.what());
        return -1;
    }
}
