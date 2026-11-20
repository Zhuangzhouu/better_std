# 示例集

[← 首页](Home.md)

仓库 [`examples/`](../examples/) 下有 5 个完整可运行示例，覆盖不同模块族。

## 编译运行

```bash
cd examples
bash build.sh          # 默认 c++11，编译并运行全部示例
bash build.sh gnu++17  # 指定标准
```

Windows / MSYS2 下脚本会自动加 `-static`。

## 示例清单

### 01_basics.cpp — 基础

高精度整数运算、快速排序、后缀数组、多项式因式分解。

```
hpn  = 86419752308641975237
sort = 1 2 3 5 7 9
sa   = 5 3 1 0 4 2
poly = (x - 1) * (x + 1)
```

### 02_graph_flow.cpp — 图

Dinic 最大流 + 2-SAT 判定。

```
maxflow = 4 (expect 4)
2sat    = SAT  assign=(1,1)
```

### 03_tree.cpp — 树

LCA / 距离、Link-Cut Tree 动态连边 + 路径加/最大、虚树压缩。

```
lca(2,3) = 0 (expect 0)
dist(2,3)= 3 (expect 3)
connected(0,3) = 1
path_max(0,3)  = 4 (expect 4)
after add max  = 14 (expect 14)
virtual tree nodes = 3
```

### 04_number_theory.cpp — 数论

CRT、Pollard-Rho 大整数分解、约数个数、BM 线性递推求斐波那契第 30 项。

```
crt      = 8 (mod 15), expect 8 (mod 15)
factors  = 71^1 839^1 1471^1 6857^1  count=16
fib[30]  = 832040 (BM rec len=2, expect 832040)
```

### 05_strings_hash.cpp — 字符串

AC 自动机多模式匹配、双哈希子串判定、3SUM 计数。

```
ac match = she:1 he:1 e:1
hash eq  = 1 ("ab"@0 vs "ab"@7)
3sum cnt = 1 (target=4, expect 1)
```

## 回归测试

[`tests/test_all.cpp`](../tests/test_all.cpp) 是 775 项对拍测试（含随机暴力对照），一键跑 10 种标准：

```bash
cd tests
bash run_tests.sh
```

输出示例：

```
=== ISO (pedantic) ===
c++11     RESULT checks=775 failures=0 PASS
...
=== GNU ===
gnu++11   RESULT checks=775 failures=0 PASS
...
ALL 10 STANDARDS PASS
```
