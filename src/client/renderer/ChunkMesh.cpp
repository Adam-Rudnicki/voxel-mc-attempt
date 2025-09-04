#include <vector>

#include "ChunkMesh.h"
#include "BufferPool.h"

using namespace mc::gfx;

ChunkMesh::ChunkMesh(const world::Chunk &chunk,
                     const std::array<world::Chunk *, world::DIRECTIONS_COUNT> &neighbors,
                     TextureAtlas &textureAtlas) {
    layers = world::Mesher::buildChunkMeshLayers(chunk, neighbors, textureAtlas);

    for (int layer = 0; layer < NUM_RENDER_LAYERS; ++layer) {
        vao_[layer] = 0;
        index_count_[layer] = static_cast<std::uint32_t>(layers.indices[layer].size());
    }

    if (isEmpty()) return;

    aabb_min_ = chunk.coord() * world::CHUNK_SIZE_VEC;
    aabb_max_ = aabb_min_ + world::CHUNK_SIZE_VEC;
    aabb_center_ = (aabb_min_ + aabb_max_) / 2;
}

void ChunkMesh::buildLayers() {
    for (int layer = 0; layer < NUM_RENDER_LAYERS; ++layer)
        if (index_count_[layer] != 0) buildLayer(layer);
}

void ChunkMesh::buildLayer(int layer) {
    GLuint &vao = vao_[layer];
    GLuint &vbo = vbo_[layer];
    GLuint &ebo = ebo_[layer];

    glCreateVertexArrays(1, &vao);
    vbo = BufferPool::instance().acquire(GL_ARRAY_BUFFER);
    ebo = BufferPool::instance().acquire(GL_ELEMENT_ARRAY_BUFFER);

    glVertexArrayVertexBuffer(vao, 0, vbo, 0, sizeof(world::Vertex));

    glEnableVertexArrayAttrib(vao, 0); // position
    glEnableVertexArrayAttrib(vao, 1); // normal
    glEnableVertexArrayAttrib(vao, 2); // uv

    glVertexArrayAttribIFormat(vao, 0, 3,GL_UNSIGNED_BYTE, offsetof(world::Vertex, position));
    glVertexArrayAttribIFormat(vao, 1, 1,GL_UNSIGNED_BYTE, offsetof(world::Vertex, packed_normal));
    glVertexArrayAttribFormat(vao, 2, 2,GL_UNSIGNED_SHORT, GL_TRUE, offsetof(world::Vertex, uv));

    glVertexArrayAttribBinding(vao, 0, 0);
    glVertexArrayAttribBinding(vao, 1, 0);
    glVertexArrayAttribBinding(vao, 2, 0);

    glVertexArrayElementBuffer(vao, ebo);

    std::size_t &vbo_capacity = vbo_capacity_[layer];
    std::size_t &ebo_capacity = ebo_capacity_[layer];

    std::vector<world::Vertex> &vertices = layers.vertices[layer];
    std::vector<std::uint32_t> &indices = layers.indices[layer];

    vbo_capacity = vertices.size() * sizeof(world::Vertex);
    ebo_capacity = indices.size() * sizeof(std::uint32_t);
    glNamedBufferData(vbo, vbo_capacity, vertices.data(), GL_STATIC_DRAW);
    glNamedBufferData(ebo, ebo_capacity, indices.data(), GL_STATIC_DRAW);

    layers = {};
}


void ChunkMesh::rebuild(const world::Chunk &chunk,
                        const std::array<world::Chunk *, world::DIRECTIONS_COUNT> &neighbors,
                        TextureAtlas &textureAtlas) {
    layers = world::Mesher::buildChunkMeshLayers(chunk, neighbors, textureAtlas);

    for (int layer = 0; layer < NUM_RENDER_LAYERS; ++layer)
        index_count_[layer] = static_cast<std::uint32_t>(layers.indices[layer].size());

    if (isEmpty()) return;

    auto upload = [](GLuint buffer, std::size_t &capacity, GLsizeiptr bytes, const void *data) {
        if (bytes > capacity) {
            glNamedBufferData(buffer, bytes, data, GL_DYNAMIC_DRAW); // orphan/resize
            capacity = bytes;
        } else
            glNamedBufferSubData(buffer, 0, bytes, data); // reuse storage
    };

    for (int layer = 0; layer < NUM_RENDER_LAYERS; ++layer) {
        if (index_count_[layer] == 0 && vao_[layer] != 0) {
            releaseLayer(layer);
            vao_[layer] = 0;
        } else {
            std::vector<world::Vertex> &vertices = layers.vertices[layer];
            std::vector<std::uint32_t> &indices = layers.indices[layer];

            if (vao_[layer] == 0) buildLayer(layer);
            else {
                upload(vbo_[layer], vbo_capacity_[layer], vertices.size() * sizeof(world::Vertex), vertices.data());
                upload(ebo_[layer], ebo_capacity_[layer], indices.size() * sizeof(std::uint32_t), indices.data());
            }
        }
    }

    layers = {};
}

void ChunkMesh::releaseLayer(int layer) const {
    BufferPool::instance().release(GL_ARRAY_BUFFER, vbo_[layer]);
    BufferPool::instance().release(GL_ELEMENT_ARRAY_BUFFER, ebo_[layer]);
    glDeleteVertexArrays(1, &vao_[layer]);
}

ChunkMesh::~ChunkMesh() {
    for (int layer = 0; layer < NUM_RENDER_LAYERS; ++layer)
        if (vao_[layer] != 0) releaseLayer(layer);
}

void ChunkMesh::drawInternal(int layer) const {
    glBindVertexArray(vao_[layer]);
    glDrawElements(GL_TRIANGLES, index_count_[layer], GL_UNSIGNED_INT, nullptr);
}

void ChunkMesh::drawOccluding() const {
    if (index_count_[0] > 0) drawInternal(0);
}

void ChunkMesh::drawCutout() const {
    if (index_count_[1] > 0) drawInternal(1);
}
