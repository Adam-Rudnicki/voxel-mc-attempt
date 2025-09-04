#include <functional>

#include "ChunkColumn.h"

using namespace mc::world;

void ChunkColumn::linkNeighbor(Direction direction, ChunkColumn &other) {
    uint8_t index = horizontalDirectionToIndex(direction);
    if (neighbors_[index] == &other) return;

    neighbors_[index] = &other;

    uint8_t other_index = horizontalDirectionToIndex(oppositeDirection(direction));
    other.neighbors_[other_index] = this;
}

void ChunkColumn::reset(const glm::ivec2 &newCoord) {
    coord_ = newCoord;
    for (auto &chunk: chunks_) chunk.reset();
    neighbors_.fill(nullptr);
}

std::pair<Chunk *, const ChunkColumn *> ChunkColumn::adjacentChunkAndColumn(Direction direction, int index) const {
    switch (direction) {
        case Direction::PositiveY:
            if (index + 1 < CHUNKS_PER_COLUMN) return {chunks_[index + 1].get(), this};
            break;
        case Direction::NegativeY:
            if (index - 1 >= 0) return {chunks_[index - 1].get(), this};
            break;
        default:
            if (ChunkColumn *neighbor = neighbors_[horizontalDirectionToIndex(direction)])
                return {neighbor->chunks_[index].get(), neighbor};
    }
    return {nullptr, nullptr};
}

std::array<Chunk *, DIRECTIONS_COUNT> ChunkColumn::adjacentChunks(int index) const {
    std::array<Chunk *, DIRECTIONS_COUNT> adjacent_chunks{};

    for (Direction direction: HORIZONTAL_DIRECTIONS)
        if (ChunkColumn *neighbor = neighbors_[horizontalDirectionToIndex(direction)])
            adjacent_chunks[directionToIndex(direction)] = neighbor->chunks_[index].get();

    adjacent_chunks[directionToIndex(Direction::NegativeY)] = index > 0 ? chunks_[index - 1].get() : nullptr;
    adjacent_chunks[directionToIndex(Direction::PositiveY)] = index < CHUNKS_PER_COLUMN - 1
                                                                  ? chunks_[index + 1].get()
                                                                  : nullptr;
    return adjacent_chunks;
}
