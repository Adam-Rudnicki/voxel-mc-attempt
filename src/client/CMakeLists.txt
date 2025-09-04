find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(asio CONFIG REQUIRED)

file(GLOB_RECURSE CLIENT_SRC CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

add_executable(minecraft-clone_client ${CLIENT_SRC}
        renderer/Application.cpp
        renderer/Application.h
        renderer/InputSystem.cpp
        renderer/InputSystem.h
        renderer/Player.cpp
        renderer/Player.h
        renderer/MeshColumn.cpp
        renderer/MeshColumn.h
        renderer/Vertex.h
)

file(COPY renderer/shaders DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/renderer)
file(COPY ${CMAKE_SOURCE_DIR}/resources/textures/atlases DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/resources/textures)

target_link_libraries(minecraft-clone_client PRIVATE
        common
        glad::glad
        glfw
        glm::glm-header-only
        spdlog::spdlog_header_only
        asio::asio
)

target_include_directories(minecraft-clone_client PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)