# 高级算法

[← 首页](Home.md) · [模块全览](Module-Catalog.md)

## 多项式

```cpp
ntt(a, invert, mod, root);        // NTT（默认 mod=998244353, root=3）
fft(a, invert);                   // FFT
fft_multiply(a, b);               // 实数多项式乘法
poly_inv(a, n, mod);
poly_ln(a, n, mod);
poly_exp(a, n, mod);
```

`std_multipoint.hpp`：

```cpp
auto vals = multipoint_eval(f, xs);       // 多点求值
auto f = interpolate(xs, ys);             // 快速插值（O(n log²n)）
```

`std_poly_factor.hpp`：多元多项式因式分解

```cpp
factorize("x^2-1");               // → "(x - 1) * (x + 1)"
```

## FWT / FST 卷积

```cpp
fwt_and(a, invert); fwt_or(a, invert); fwt_xor(a, invert);
and_conv(a, b); or_conv(a, b); xor_conv(a, b);   // 三种集合卷积
fst_subset(a, invert); fst_superset(a, invert);   // 子集/超集 zeta 变换
subset_conv(f, g, n);             // 子集卷积，n=元素个数（数组长 2^n）
```

> `subset_conv` 用 O(3ⁿ) 枚举子集实现（n≤18 够快且可靠），zeta 分层版本已弃用。

## 数值计算

```cpp
simpson(f, l, r);
adaptive_simpson(f, l, r, eps, maxdepth);
bisect_root(f, l, r, eps);
newton_root(f, df, x0, eps);
lagrange_interpolate(xs, ys, x0);         // 实数
lagrange_interpolate_mod(ys, x0);         // 模素数，等距 x=0,1,2...
simplex(A, b, c);                          // 单纯形法，返回 simplex_result
```

## KD-Tree

```cpp
std::vector<std::vector<int>> pts = {{1,2},{3,4},{5,6}};
kd_tree<int> kd(pts);
auto res = kd.nearest({2,3});     // {最近点, 平方距离}
kd.range_count(lo, hi);           // 矩形内点数
```

> 内部 DFS 递归构建子树包围盒，最近邻按包围盒距离剪枝。

## 离线技巧

```cpp
// 整体二分：区间第 k 小
struct ob_query{ int l, r, k, id; };   // 0-based 闭区间 [l,r]
auto ans = overall_binary_range_kth(a, qs, vlo, vhi);

// 线段树分治：动态二分图（加边有时间区间）
struct edge_interval{ int u, v, l, r; };
auto ok = segment_tree_divide_bipartite(n, T, edges);
```

## 3SUM 家族

```cpp
two_sum(a, target);               // 一组下标
three_sum(a, target);             // 去重三元组
three_sum_closest(a, target);     // 最接近的三元组 + 实际和
three_sum_count(a, target);       // 计数版（下标不同即不同组合）
four_sum(a, target);              // 去重四元组
three_sum_exists_bitset(a, target); // bitset 位压存在性判定 O(n·M/64)
```

> `three_sum_exists_bitset` 判定的是“值集合能否凑出三数”（值可重复取），与去重三元组语义不同；值域跨度 >8e6 会自动放弃退回经典算法。

## 博弈论

```cpp
nim_xor(piles);                   // Nim 和
nim_win(piles);                   // 是否必胜
nim_first_move(piles);            // {取哪堆, 取多少个}
sg_dag(n, edges);                 // DAG 上 SG 函数（要求后继编号 < u）
bash_game(n, m);                  // 巴什博弈
stairs_nim(a);                    // 阶梯 Nim
```

## 其他

| 模块 | 内容 |
|---|---|
| `std_geometry.hpp` | 凸包、鞋带面积、线段相交、点在多边形、矩形面积并 |
| `std_matrix_tree.hpp` | 矩阵树定理生成树计数 |
| `std_dp_opt.hpp` | 李超线段树 `li_chao_tree`、凸壳优化 `convex_hull_trick_min/max` |
| `std_cdq.hpp` | `cdq_3d_partial_order` 三维偏序 + 通用 `cdq_divide_conquer` |
| `std_monotonic.hpp` | 最大矩形、接雨水、单调队列最值 |
