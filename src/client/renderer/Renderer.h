#pragma once
#include <memory>
#include <optional>

#include "Shader.h"
#include "ChunkMesh.h"
#include "MeshColumn.h"
#include "TextureAtlas.h"
#include "../../common/core/Camera.h"
#include "../../common/world/World.h"

namespace mc::gfx {
    class Renderer {
    public:
        Renderer();

        ~Renderer() = default;

        void renderFrame(const core::Camera &camera);

        void regenerateTerrain(int seed);

        void toggleTerrainGenerationMode();

        bool breakBlock(const glm::ivec3 &worldCoord);

        bool placeBlock(const glm::ivec3 &worldCoord, world::BlockId blockId);

        void streamMeshColumns(const core::Camera &camera);

        const world::World &world() const { return world_; }

        void setHighlightBlock(const std::optional<glm::ivec3> &block) { highlight_block_ = block; }

    private:
        world::World world_;

        struct {
            // default shader (chunks)
            GLint u_MVP = -1;
            GLint u_texture = -1;
            GLint u_light_direction = -1;
            GLint u_fog_color = -1;
            GLint u_fog_start = -1;
            GLint u_fog_end = -1;
            GLint u_camera_position = -1;
            GLint u_chunk_origin = -1;
            // outline shader (blocks outline)
            GLint outline_u_MVP = -1;
            GLint outline_u_offset = -1;
            GLint outline_u_color = -1;
            // hud shader (cross-hair)
            GLint hud_u_color = -1;
        } uniforms_;

        TextureAtlas texture_atlas_;

        std::unordered_map<glm::ivec2,
            std::unique_ptr<MeshColumn>,
            world::ColumnHash> mesh_columns_;

        Shader default_shader_;

        Shader outline_shader_;
        GLuint outline_vao_ = 0, outline_vbo_ = 0;
        std::optional<glm::ivec3> highlight_block_ = std::nullopt;

        Shader hud_shader_;
        GLuint hud_vao_ = 0, hud_vbo_ = 0;

        void initUniformLocations();

        void createMeshColumn(const world::ChunkColumn &column);

        void updateChunkMesh(const world::Chunk &chunk,
                             const world::ChunkColumn &column);
    };
}
