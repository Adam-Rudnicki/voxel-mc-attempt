#include <asio.hpp>
#include <spdlog/spdlog.h>

int main() {
    asio::io_context context;
    spdlog::info("minecraft-clone headless server starting...");
    context.run();
}
