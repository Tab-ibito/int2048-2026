#!/usr/bin/env python3
"""运行 python3 generate_submit.py，将两个源文件合并为仓库根目录的 submit.h。"""

from pathlib import Path


def main():
    root = Path(__file__).resolve().parent
    header = (root / "src/include/int2048.h").read_text(encoding="utf-8")
    source = (root / "src/int2048.cpp").read_text(encoding="utf-8")

    header = header.rstrip()
    if not header.endswith("\n#endif"):
        raise SystemExit("合并失败：int2048.h 必须以单独一行 #endif 结束。")

    source_lines = source.splitlines(keepends=True)
    includes = {'#include "include/int2048.h"', '#include "int2048.h"'}
    if not any(line.strip() in includes for line in source_lines):
        raise SystemExit("合并失败：int2048.cpp 中未找到对 int2048.h 的引用。")

    implementation = "".join(
        line for line in source_lines if line.strip() not in includes
    )
    merged = (
        header[:-len("#endif")].rstrip()
        + "\n\n"
        + implementation.strip()
        + "\n\n#endif\n"
    )

    output = root / "submit.h"
    output.write_text(merged, encoding="utf-8")
    print(f"已生成：{output}")


if __name__ == "__main__":
    main()
