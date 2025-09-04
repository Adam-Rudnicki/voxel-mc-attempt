from typing import Dict, Tuple

GRASS_BLOCK_TOP = "grass_block_top.png"
DIRT = "dirt.png"
STONE = "stone.png"
BEDROCK = "bedrock.png"
OAK_LOG = "oak_log.png"
OAK_LEAVES = "oak_leaves.png"

TILE_POS: Dict[str, Tuple[int, int]] = {
    GRASS_BLOCK_TOP: (0, 0),
    DIRT: (1, 0),
    STONE: (2, 0),
    BEDROCK: (3, 0),
    OAK_LOG: (4, 0),
    OAK_LEAVES: (5, 0),
}
