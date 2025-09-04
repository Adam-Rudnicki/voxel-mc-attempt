#include "MeshColumn.h"

using namespace mc::gfx;

void MeshColumn::generate(const world::ChunkColumn &chunkColumn, TextureAtlas &atlas) {
    for (auto &mesh_ptr: meshes_) mesh_ptr.reset();

    for (int i = 0; i < world::CHUNKS_PER_COLUMN; ++i) {
        auto &chunk_ptr = chunkColumn.chunks()[i];
        if (!chunk_ptr) continue;

        auto neighbours = chunkColumn.adjacentChunks(i);
        auto mesh = std::make_unique<ChunkMesh>(*chunk_ptr, neighbours, atlas);

        if (!mesh->isEmpty()) meshes_[i] = std::move(mesh);
    }
}

void MeshColumn::rebuildMesh(const world::Chunk &chunk,
                             const world::ChunkColumn &chunkColumn,
                             TextureAtlas &atlas) {
    int index = chunk.coord().y;
    auto neighbours = chunkColumn.adjacentChunks(index);
    auto &mesh_ptr = meshes_[index];

    if (mesh_ptr) mesh_ptr->rebuild(chunk, neighbours, atlas);
    else mesh_ptr = std::make_unique<ChunkMesh>(chunk, neighbours, atlas);

    if (mesh_ptr->isEmpty()) mesh_ptr.reset();
}
