find_package(glm CONFIG REQUIRED)

file(GLOB_RECURSE COMMON_SRC CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

add_library(common STATIC ${COMMON_SRC}
        world/WorldConstants.h
        world/ChunkColumn.cpp
        world/ChunkColumn.h
        world/Direction.h
)

target_include_directories(common PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(common PUBLIC
        glm::glm-header-only
)
