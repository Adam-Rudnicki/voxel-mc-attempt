from pathlib import Path

ROOT_DIR = Path(__file__).parent.resolve()
SCAN_DIR = ROOT_DIR / "src"
OUTPUT_FILE = ROOT_DIR / "concat.txt"

_FILES_TO_SKIP = {Path(__file__).resolve(), OUTPUT_FILE.resolve()}


def is_non_empty_file(p: Path) -> bool:
    return p.is_file() and p.stat().st_size > 0 and p.resolve() not in _FILES_TO_SKIP


def append_file(path: Path, out_handle) -> None:
    relative_path = path.relative_to(ROOT_DIR)
    out_handle.write(f"--- BEGIN {relative_path} ---\n")
    with path.open("r", encoding="utf-8", errors="replace") as in_file:
        content = in_file.read()
        out_handle.write(content)
        if not content.endswith("\n"):
            out_handle.write("\n")
    out_handle.write(f"--- END {relative_path} ---\n\n")


def crawl_and_write() -> None:
    with OUTPUT_FILE.open("w", encoding="utf-8", errors="replace") as out_file:
        for path in ROOT_DIR.iterdir():
            if is_non_empty_file(path):
                append_file(path, out_file)

        if SCAN_DIR.is_dir():
            for path in SCAN_DIR.rglob("*"):
                if is_non_empty_file(path):
                    append_file(path, out_file)


if __name__ == "__main__":
    crawl_and_write()
