find_package(asio CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)

file(GLOB_RECURSE SERVER_SRC CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

add_executable(minecraft-clone_server ${SERVER_SRC})

target_link_libraries(minecraft-clone_server PRIVATE
        asio::asio
        spdlog::spdlog_header_only
)

target_include_directories(minecraft-clone_server PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
