#pragma once
#include <array>
#include <memory>

#include "../../common/world/WorldConstants.h"
#include "../../common/world/ChunkColumn.h"
#include "ChunkMesh.h"
#include "TextureAtlas.h"

namespace mc::gfx {
    class MeshColumn {
    public:
        MeshColumn() = delete;

        MeshColumn(const world::ChunkColumn &chunkColumn, TextureAtlas &atlas) {
            generate(chunkColumn, atlas);
        }

        void rebuildMesh(const world::Chunk &chunk,
                         const world::ChunkColumn &chunkColumn,
                         TextureAtlas &atlas);

        auto &meshes() { return meshes_; }
        const auto &meshes() const { return meshes_; }

    private:
        std::array<std::unique_ptr<ChunkMesh>, world::CHUNKS_PER_COLUMN> meshes_{};

        void generate(const world::ChunkColumn &chunkColumn, TextureAtlas &atlas);
    };
}
