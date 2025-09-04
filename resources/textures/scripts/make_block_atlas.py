from pathlib import Path
from typing import Dict, Tuple
import logging
from colorama import init, Fore, Style
from PIL import Image
import block_tiles as bt
import sys

init(autoreset=True)


# ────────── logging ──────────
class AnsiColorFormatter(logging.Formatter):
    COLORS = {
        logging.DEBUG: Fore.CYAN,
        logging.INFO: Fore.GREEN,
        logging.WARNING: Fore.YELLOW,
        logging.ERROR: Fore.RED,
        logging.CRITICAL: Fore.MAGENTA + Style.BRIGHT,
    }

    def format(self, record):
        color = self.COLORS.get(record.levelno, "")
        msg = super().format(record)
        return f"{color}{msg}{Style.RESET_ALL}"


logger = logging.getLogger("make_atlas")
handler = logging.StreamHandler()
handler.setLevel(logging.INFO)
formatter = AnsiColorFormatter('{asctime} | {levelname:<8s} | {name:<20s} | {message}', style='{')
handler.setFormatter(formatter)
logger.addHandler(handler)
logger.setLevel(logging.DEBUG)

# ────────── config ──────────
TILE_PX = 16
GRID = 16
ATLAS_PX = TILE_PX * GRID
OUT_NAME = "block_atlas.png"

TILE_POS: Dict[str, Tuple[int, int]] = bt.TILE_POS

if len(sys.argv) < 2:
    logger.error("Provide the path to the directory with block textures as an argument!")
    sys.exit(1)

TEX_DIR = Path(sys.argv[1]).resolve()
if not TEX_DIR.exists() or not TEX_DIR.is_dir():
    logger.error(f"The directory {TEX_DIR} does not exist or is not a directory!")
    sys.exit(1)

atlas_dir = TEX_DIR.parent / "atlases"
atlas_dir.mkdir(exist_ok=True)

# ────────── build atlas ──────────
atlas = Image.new("RGBA", (ATLAS_PX, ATLAS_PX))

for fname, (col, row) in TILE_POS.items():
    path = TEX_DIR / fname
    if not path.exists():
        logger.warning(f"missing {fname} – left cell blank")
        continue
    img = Image.open(path).convert("RGBA")
    if img.size != (TILE_PX, TILE_PX):
        logger.warning(f"{fname}: expected 16×16, got {img.size} – skipped")
        continue

    atlas.paste(img, (col * TILE_PX, row * TILE_PX))

# ────────── save & print ──────────
atlas_path = atlas_dir / OUT_NAME
atlas.save(atlas_path)
logger.info(f"atlas written → {atlas_path}\n")
