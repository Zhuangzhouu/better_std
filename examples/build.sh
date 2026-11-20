#!/usr/bin/env bash
# 编译 examples/ 下所有示例并运行。用法：bash build.sh [std]，默认 c++11。
set -u
cd "$(dirname "$0")"
STD="${1:-c++11}"
CXX="${CXX:-g++}"

# MSYS2 场景：确保编译器所在目录在 PATH（否则 cc1plus.exe 缺 DLL）
CXXDIR="$(dirname "$(command -v "$CXX" 2>/dev/null || printf '%s' "$CXX")")"
export PATH="$CXXDIR:$PATH"

# Windows/MSYS2 下静态链接，避免 exe 缺运行库（报 127）
STATIC=""
case "$(uname -s 2>/dev/null)" in
  MINGW*|MSYS*|CYGWIN*) STATIC="-static" ;;
esac

fail=0
for src in *.cpp; do
  out="${src%.cpp}"
  if "$CXX" -std="$STD" -O2 -Wall -Wextra $STATIC -o "$out" "$src" 2>"_err.txt"; then
    printf '%-24s ' "$src"
    "./$out" 2>&1 | tr '\n' ' '; echo
  else
    echo "COMPILE FAIL $src"; grep -m3 'error:' _err.txt; fail=1
  fi
done
rm -f _err.txt
exit $fail
