#include <ranges>
#include <spdlog/spdlog.h>
#include <glm/gtc/type_ptr.hpp>
#include <future>

#include "Renderer.h"
#include "../common/world/Chunk.h"
#include "core/Frustum.h"

using namespace mc::gfx;
using mc::world::Chunk;

Renderer::Renderer()
    : texture_atlas_("resources/textures/atlases/block_atlas.png"),
      default_shader_("renderer/shaders/basic.vert",
                      "renderer/shaders/basic.frag"),
      outline_shader_("renderer/shaders/outline.vert",
                      "renderer/shaders/outline.frag"),
      hud_shader_("renderer/shaders/hud.vert",
                  "renderer/shaders/hud.frag") {
    initUniformLocations();
    mesh_columns_.reserve(world::RENDER_AREA_SIZE);

    // --- build a unit-cube edge VAO (24 vertices) --------------------------
    constexpr float v[72] = {
        0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, // bottom
        0, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 0, // top
        0, 0, 0, 0, 1, 0, 1, 0, 0, 1, 1, 0, 1, 0, 1, 1, 1, 1, 0, 0, 1, 0, 1, 1 // verticals
    };
    glCreateVertexArrays(1, &outline_vao_);
    glCreateBuffers(1, &outline_vbo_);
    glNamedBufferData(outline_vbo_, sizeof(v), v, GL_STATIC_DRAW);

    glVertexArrayVertexBuffer(outline_vao_, 0, outline_vbo_, 0, 3 * sizeof(float));
    glEnableVertexArrayAttrib(outline_vao_, 0);
    glVertexArrayAttribFormat(outline_vao_, 0, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(outline_vao_, 0, 0);

    // --- cross-hair HUD element ---------------------------------------------
    glm::vec2 crosshair_vertices[4] = {
        {-0.015f, 0.0f}, {0.015f, 0.0f}, // horizontal
        {0.0f, -0.015f}, {0.0f, 0.015f} // vertical
    };
    glCreateVertexArrays(1, &hud_vao_);
    glCreateBuffers(1, &hud_vbo_);
    glNamedBufferData(hud_vbo_, sizeof(crosshair_vertices), crosshair_vertices, GL_STATIC_DRAW);

    glVertexArrayVertexBuffer(hud_vao_, 0, hud_vbo_, 0, sizeof(glm::vec2));
    glEnableVertexArrayAttrib(hud_vao_, 0);
    glVertexArrayAttribFormat(hud_vao_, 0, 2, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(hud_vao_, 0, 0);
}

void Renderer::initUniformLocations() {
    uniforms_.u_MVP = glGetUniformLocation(default_shader_.id(), "uMVP");
    uniforms_.u_texture = glGetUniformLocation(default_shader_.id(), "uTexture");
    uniforms_.u_light_direction = glGetUniformLocation(default_shader_.id(), "uLightDirection");
    uniforms_.u_fog_color = glGetUniformLocation(default_shader_.id(), "uFogColor");
    uniforms_.u_fog_start = glGetUniformLocation(default_shader_.id(), "uFogStart");
    uniforms_.u_fog_end = glGetUniformLocation(default_shader_.id(), "uFogEnd");
    uniforms_.u_camera_position = glGetUniformLocation(default_shader_.id(), "uCameraPosition");
    uniforms_.u_chunk_origin = glGetUniformLocation(default_shader_.id(), "uChunkOrigin");

    uniforms_.outline_u_MVP = glGetUniformLocation(outline_shader_.id(), "uMVP");
    uniforms_.outline_u_offset = glGetUniformLocation(outline_shader_.id(), "uOffset");
    uniforms_.outline_u_color = glGetUniformLocation(outline_shader_.id(), "uColor");

    uniforms_.hud_u_color = glGetUniformLocation(hud_shader_.id(), "uColor");
}

void Renderer::renderFrame(const core::Camera &camera) {
    texture_atlas_.bind(0);
    glm::mat4 vp = camera.viewProjection();

    core::Frustum frustum(vp);
    std::vector<std::pair<float, ChunkMesh *> > visible_meshes;
    visible_meshes.reserve(world::RENDER_AREA_SIZE * world::CHUNKS_PER_COLUMN / 4);

    for (auto &mesh_column: mesh_columns_ | std::views::values)
        for (auto &mesh_ptr: mesh_column->meshes())
            if (mesh_ptr && frustum.intersectsAABB(mesh_ptr->aabb_min(), mesh_ptr->aabb_max())) {
                glm::vec3 diff = camera.position() - glm::vec3(mesh_ptr->aabb_center());
                visible_meshes.emplace_back(glm::dot(diff, diff), mesh_ptr.get());
            }

    std::ranges::sort(visible_meshes, [](auto &a, auto &b) { return a.first < b.first; });

    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);

    default_shader_.use();
    glUniformMatrix4fv(uniforms_.u_MVP, 1, GL_FALSE, glm::value_ptr(vp));
    glUniform1i(uniforms_.u_texture, 0);
    glm::vec3 light_dir = glm::normalize(glm::vec3(0.5f, 1.0f, 0.3f));
    glUniform3fv(uniforms_.u_light_direction, 1, glm::value_ptr(light_dir));
    constexpr auto fog_color = glm::vec3(0.73f, 0.80f, 0.85f);
    glUniform3fv(uniforms_.u_fog_color, 1, glm::value_ptr(fog_color));
    glUniform1f(uniforms_.u_fog_start, 32.0f);
    glUniform1f(uniforms_.u_fog_end, 1000.0f);
    glUniform3fv(uniforms_.u_camera_position, 1, glm::value_ptr(camera.position()));

    for (ChunkMesh *mesh: visible_meshes | std::views::values) {
        glUniform3iv(uniforms_.u_chunk_origin, 1, glm::value_ptr(mesh->origin()));
        mesh->drawOccluding();
    }

    // ---------- Main colour : cutout pass (double-sided) -----------
    glDisable(GL_CULL_FACE);
    glDepthFunc(GL_LEQUAL);
    for (ChunkMesh *mesh: visible_meshes | std::views::values) {
        glUniform3iv(uniforms_.u_chunk_origin, 1, glm::value_ptr(mesh->origin()));
        mesh->drawCutout();
    }
    glEnable(GL_CULL_FACE);
    glDepthFunc(GL_LESS);

    if (highlight_block_) {
        glDisable(GL_DEPTH_TEST);

        outline_shader_.use();
        glUniformMatrix4fv(uniforms_.outline_u_MVP, 1, GL_FALSE, glm::value_ptr(vp));
        glUniform3f(uniforms_.outline_u_offset, highlight_block_->x, highlight_block_->y, highlight_block_->z);
        glUniform3f(uniforms_.outline_u_color, 0.0f, 0.0f, 0.0f); // black
        glBindVertexArray(outline_vao_);
        glDrawArrays(GL_LINES, 0, 24); // 12 edges * 2 vertices each

        glEnable(GL_DEPTH_TEST);
    }

    hud_shader_.use();
    glDepthMask(GL_FALSE);
    glDisable(GL_DEPTH_TEST);
    glUniform3f(uniforms_.hud_u_color, 0.95f, 0.95f, 0.95f); // light gray
    glBindVertexArray(hud_vao_);
    glDrawArrays(GL_LINES, 0, 4);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
}

void Renderer::regenerateTerrain(int seed) {
    mesh_columns_.clear();
    world_.setSeed(seed);
}

void Renderer::toggleTerrainGenerationMode() {
    mesh_columns_.clear();
    world_.toggleTerrainMode();
    spdlog::info("Terrain mode is now {}", world_.terrain_generation_mode() == world::TerrainGenerationMode::SineWave
                                               ? "Sine-wave"
                                               : "Perlin noise");
}

void Renderer::streamMeshColumns(const core::Camera &camera) {
    auto start = std::chrono::high_resolution_clock::now();

    glm::ivec2 centre = world_.worldToColumn(glm::floor(camera.position()));
    std::vector<world::ChunkColumn *> created_chunk_columns = world_.streamChunkColumns(centre);
    if (created_chunk_columns.empty()) return;

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    spdlog::info("ChunkColumn streaming took {} ms", duration.count());

    std::erase_if(mesh_columns_, [&](const auto &kv) {
        glm::ivec2 d = kv.first - centre;
        return d.x * d.x + d.y * d.y > world::RENDER_RADIUS * world::RENDER_RADIUS;
    });

    std::vector<std::future<std::pair<glm::ivec2, std::unique_ptr<MeshColumn> > > > futures;

    for (auto &offset: world::RENDER_RADIUS_OFFSETS) {
        glm::ivec2 column_coord = centre + offset;
        if (!mesh_columns_.contains(column_coord)) {
            auto &column = world_.chunk_columns().at(column_coord);

            futures.emplace_back(std::async(
                std::launch::async, [this, &column]() -> std::pair<glm::ivec2, std::unique_ptr<MeshColumn> > {
                    auto mesh_column = std::make_unique<MeshColumn>(*column, texture_atlas_);
                    return {column->coord(), std::move(mesh_column)};
                }));
        }
    }

    for (auto &f: futures) {
        auto [coord, mesh_column] = f.get();

        for (auto &mesh_ptr: mesh_column->meshes())
            if (mesh_ptr) mesh_ptr->buildLayers();

        mesh_columns_.emplace(coord, std::move(mesh_column));
    }

    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    spdlog::info("MeshColumn generation took {} ms", duration.count());
}

bool Renderer::breakBlock(const glm::ivec3 &worldCoord) {
    world::ChunkLookup lookup = world_.chunkLookup(worldCoord);
    if (!mesh_columns_.contains(lookup.chunk_column->coord())) return false;
    if (!lookup.chunk || !lookup.chunk->blockAt(lookup.local_coord).opaque()) return false;

    lookup.chunk->setBlock(lookup.local_coord, world::BlockId::Air);

    if (lookup.chunk->isEmpty()) {
        mesh_columns_[lookup.chunk_column->coord()]->meshes()[lookup.index].reset();
        lookup.chunk_column->chunks()[lookup.index].reset();
    } else updateChunkMesh(*lookup.chunk, *lookup.chunk_column);

    auto rebuildNeighbor = [&](world::Direction direction) {
        auto [chunk, column] = lookup.chunk_column->adjacentChunkAndColumn(direction, lookup.index);
        if (chunk) updateChunkMesh(*chunk, *column);
    };

    if (lookup.local_coord.x == 0) rebuildNeighbor(world::Direction::NegativeX);
    else if (lookup.local_coord.x == world::CHUNK_XYZ - 1) rebuildNeighbor(world::Direction::PositiveX);
    if (lookup.local_coord.y == 0) rebuildNeighbor(world::Direction::NegativeY);
    else if (lookup.local_coord.y == world::CHUNK_XYZ - 1) rebuildNeighbor(world::Direction::PositiveY);
    if (lookup.local_coord.z == 0) rebuildNeighbor(world::Direction::NegativeZ);
    else if (lookup.local_coord.z == world::CHUNK_XYZ - 1) rebuildNeighbor(world::Direction::PositiveZ);

    return true;
}

bool Renderer::placeBlock(const glm::ivec3 &worldCoord, world::BlockId blockId) {
    world::ChunkLookup lookup = world_.chunkLookup(worldCoord);
    if (!mesh_columns_.contains(lookup.chunk_column->coord())) return false;
    if (!lookup.chunk_column || lookup.index < 0) return false;
    if (!lookup.chunk && blockId == world::BlockId::Air) return false;
    if (lookup.chunk && lookup.chunk->blockAt(lookup.local_coord).id == blockId) return false;

    if (lookup.chunk) {
        lookup.chunk->setBlock(lookup.local_coord, blockId);
        updateChunkMesh(*lookup.chunk, *lookup.chunk_column);
    } else {
        auto &chunk_ptr = lookup.chunk_column->chunks()[lookup.index];
        glm::ivec3 chunk_coord = {lookup.chunk_column->coord().x, lookup.index, lookup.chunk_column->coord().y};
        chunk_ptr = std::make_unique<Chunk>(chunk_coord);
        chunk_ptr->setBlock(lookup.local_coord, blockId);
        updateChunkMesh(*chunk_ptr, *lookup.chunk_column);
    }

    auto rebuildNeighbor = [&](world::Direction direction) {
        auto [chunk, column] = lookup.chunk_column->adjacentChunkAndColumn(direction, lookup.index);
        if (chunk) updateChunkMesh(*chunk, *column);
    };

    if (lookup.local_coord.x == 0) rebuildNeighbor(world::Direction::NegativeX);
    else if (lookup.local_coord.x == world::CHUNK_XYZ - 1) rebuildNeighbor(world::Direction::PositiveX);
    if (lookup.local_coord.y == 0) rebuildNeighbor(world::Direction::NegativeY);
    else if (lookup.local_coord.y == world::CHUNK_XYZ - 1) rebuildNeighbor(world::Direction::PositiveY);
    if (lookup.local_coord.z == 0) rebuildNeighbor(world::Direction::NegativeZ);
    else if (lookup.local_coord.z == world::CHUNK_XYZ - 1) rebuildNeighbor(world::Direction::PositiveZ);

    return true;
}

void Renderer::updateChunkMesh(const Chunk &chunk, const world::ChunkColumn &column) {
    auto it = mesh_columns_.find(column.coord());
    if (it == mesh_columns_.end()) return;

    it->second->rebuildMesh(chunk, column, texture_atlas_);
}
