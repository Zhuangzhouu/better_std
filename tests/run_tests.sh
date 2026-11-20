#!/usr/bin/env bash
# 在 10 种 C++ 标准下编译并运行 test_all，验证全标准兼容。
# 用法：bash run_tests.sh
set -u
cd "$(dirname "$0")"
CXX="${CXX:-g++}"

CXXDIR="$(dirname "$(command -v "$CXX" 2>/dev/null || printf '%s' "$CXX")")"
export PATH="$CXXDIR:$PATH"

# Windows/MSYS2 下静态链接
STATIC=""
case "$(uname -s 2>/dev/null)" in
  MINGW*|MSYS*|CYGWIN*) STATIC="-static" ;;
esac

# ISO 标准加 -pedantic-errors；GNU 标准不加（__int128 会被 pedantic 判违规）
ISOS="c++11 c++14 c++17 c++20 c++23"
GNUS="gnu++11 gnu++14 gnu++17 gnu++20 gnu++23"

fail=0
run_one() {
  std="$1"; extra="$2"
  if "$CXX" -std="$std" $extra -O2 -Wall -Wextra $STATIC -o test_all test_all.cpp 2>"_err_${std}.txt"; then
    result="$(./test_all 2>&1 | tail -1)"
    printf '%-9s %s\n' "$std" "$result"
    case "$result" in *failures=0*PASS*) ;; *) fail=1 ;; esac
  else
    printf '%-9s COMPILE FAIL\n' "$std"
    grep -m3 'error:' "_err_${std}.txt"; fail=1
  fi
  rm -f "_err_${std}.txt"
}

echo "=== ISO (pedantic) ==="
for std in $ISOS; do run_one "$std" "-pedantic-errors"; done
echo "=== GNU ==="
for std in $GNUS; do run_one "$std" ""; done

rm -f test_all
if [ "$fail" -eq 0 ]; then
  echo "ALL 10 STANDARDS PASS"
else
  echo "SOME STANDARDS FAILED"
fi
exit $fail
