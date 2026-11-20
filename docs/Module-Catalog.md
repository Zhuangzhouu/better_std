# 模块全览

[← 首页](Home.md)

82 个 `std_*.hpp` 头文件，按类别索引。点开具体分类页看用法。

## 数值与高精度

| 头文件 | 内容 |
|---|---|
| `std_goodint.hpp` | 高精度整数 `High_precision_number<accuracy>`：四则/divmod/gcd/hash/混合运算/流输入输出/pow/powmod/isqrt/factorial/choose |
| `std_gooddouble.hpp` | 高精度浮点 `High_precision_floating_point_number<accuracy>`：58 个 cmath 重载、goodint 参数版、混合运算 |
| `std_fraction.hpp` | 高精度分数 `fraction` |
| `std_complex.hpp` | 复数 `std_complex` |
| `std_matrix.hpp` | 矩阵：乘/幂/Bareiss 行列式/逆 |
| `std_polynomial.hpp` | 多项式：乘除/求导积分/NTT/FFT/inv·ln·exp |

## 容器与区间

| 头文件 | 内容 |
|---|---|
| `std_heap.hpp` | 二叉堆 `std_heap` |
| `std_bst.hpp` | 二叉搜索树 `std_bst`：kth/rank/范围/前驱后继 |
| `std_trie.hpp` | 字典树 `std_trie` |
| `std_dsu.hpp` | 并查集 `std_dsu` |
| `std_fenwick.hpp` | 树状数组 `std_fenwick`：kth/set |
| `std_segment_tree.hpp` | 线段树 `std_segment_tree`：懒标记 |
| `std_segment_tree_beats.hpp` | Segment Tree Beats：区间 chmin + 加 |
| `std_sparse_table.hpp` | ST 表 `std_sparse_table`：RMQ |
| `std_persistent_segment_tree.hpp` | 主席树：区间第 k 小 |
| `std_cat_tree.hpp` | 猫树 `cat_tree`：O(1) 区间查询，支持非幂等运算 |
| `std_linear_basis.hpp` | 异或线性基 / 前缀线性基区间最大异或 |
| `std_minmax_heap.hpp` | 双端优先队列 |
| `std_sqrt_decomposition.hpp` | 分块：区间加/和/最值/≤x 计数 |
| `std_mergeable_heap.hpp` | 左偏树 / 斜堆 / 配对堆可并堆三件套 |

## 图、网络流与树

| 头文件 | 内容 |
|---|---|
| `std_grid.hpp` 等 | 网格图基类 + 邻接表 / 邻接矩阵派生 |
| `std_grid_algorithm.hpp` | 图算法包：Dijkstra、MST、欧拉路等 |
| `std_dinic.hpp` | Dinic 最大流 `std_dinic` |
| `std_mcmf.hpp` | 最小费用最大流 `std_mcmf` |
| `std_bipartite_matching.hpp` | Hopcroft-Karp + 多重匹配 |
| `std_km.hpp` | KM 二分图最大/最小权完美匹配 |
| `std_tarjan.hpp` | 割点/桥/点双/边双 |
| `std_two_sat.hpp` | 2-SAT：Tarjan SCC 判定 + 可行赋值 |
| `std_rootless_tree.hpp` | 无根树 LCA/倍增/kth 祖先/dist |
| `std_tree.hpp` | 有根树 |
| `std_binary_tree.hpp` | 二叉树：四种遍历/高度 |
| `std_heavy_light.hpp` | 重链剖分 |
| `std_lct.hpp` | Link-Cut Tree：link/cut/路径加/路径最大 |
| `std_tree_extra.hpp` | 轻量 LCA + 树直径/重心 + 虚树 |
| `std_block_cut_tree.hpp` | 圆方树（点双）+ 仙人掌距离 |

## 字符串

| 头文件 | 内容 |
|---|---|
| `std_string_algorithm.hpp` | KMP/Z 函数/字符串哈希/Manacher |
| `std_ac_automaton.hpp` | AC 自动机：多模式匹配、fail 树 |
| `std_sa_sam.hpp` | 后缀数组 / 后缀自动机 |
| `std_string_extra.hpp` | 回文自动机 / 后缀树 / SA height 应用 |
| `std_hashing.hpp` | 单/双滚动哈希、可回滚哈希、树同构哈希 |

## 数论与组合

| 头文件 | 内容 |
|---|---|
| `std_number_theory.hpp` | modpow/exgcd/modinv/线性筛/组合数/Lucas/BSGS/原根 |
| `std_crt.hpp` | CRT / 线性同余（模数可到 1e18，compat 防溢出） |
| `std_prime_factor.hpp` | Miller-Rabin 素性 + Pollard-Rho 分解 + 约数个数/和 |
| `std_dujiao.hpp` | 杜教筛 μ/φ 前缀和 |
| `std_linear_recurrence.hpp` | BM 最短线性递推 + Kitamasa 第 n 项 |
| `std_combinatorics.hpp` | 卡特兰/斯特林/贝尔/划分/错排/容斥 |
| `std_polya.hpp` | Burnside / Polya：染色/项链/手链计数 |

## 算法工具与 DP

| 头文件 | 内容 |
|---|---|
| `std_monotonic.hpp` | 最大矩形/接雨水/单调队列最值 |
| `std_sequence.hpp` | LIS/LCS |
| `std_prefix_sum.hpp` / `std_difference.hpp` | 一维/二维前缀和与差分 |
| `std_permutation.hpp` | 全排列/康托展开/逆序对 |
| `std_digit_dp.hpp` | 数位 DP |
| `std_knapsack.hpp` | 0-1/完全/多重/子集和背包 |
| `std_cdq.hpp` | CDQ 分治：三维偏序 + 通用 CDQ |
| `std_dp_opt.hpp` | 李超线段树/凸壳/单调队列优化 DP |
| `std_mo.hpp` | 莫队 |
| `std_discretize.hpp` | 离散化 |
| `std_gaussian.hpp` | 实数/模素数/异或高斯消元 |
| `std_sort.hpp` | 插入/选择/冒泡/希尔/快排/归并/堆排/计数/基数/桶排/nth_element |
| `std_binary_search.hpp` | 二分谓词/三分搜索 |
| `std_extrema.hpp` | 前缀/后缀极值 + 极值栈 + 二维 |
| `std_type_dsu.hpp` | 种类并查集 + 带权区间 DSU |
| `std_weighted_dsu.hpp` | 带权并查集（Monoid 泛型 + XOR 别名） |
| `std_dsu_extra.hpp` | 可撤销 / 可持久化并查集、可持久化数组 |
| `std_geometry.hpp` | 点/向量、凸包、鞋带面积、线段相交、点在多边形、矩形面积并 |

## 高级专题

| 头文件 | 内容 |
|---|---|
| `std_polynomial.hpp` | 多项式 NTT/FFT/inv·ln·exp（见数值） |
| `std_multipoint.hpp` | 多项式多点求值 + 快速插值 |
| `std_poly_factor.hpp` | 多元多项式因式分解（自带 BigInt） |
| `std_fwt.hpp` | FWT(AND/OR/XOR) 卷积 + FST 子集卷积 |
| `std_numerical.hpp` | Simpson 积分、二分/牛顿求根、拉格朗日插值 |
| `std_simplex.hpp` | 单纯形法线性规划 |
| `std_matrix_tree.hpp` | 矩阵树定理生成树计数 |
| `std_kd_tree.hpp` | KD-Tree 最近邻 + 范围查询 |
| `std_offline.hpp` | 整体二分 + 线段树分治动态二分图 |
| `std_three_sum.hpp` | 2/3/4SUM、计数版、bitset 位压存在性判定 |
| `std_game_theory.hpp` | Nim/SG/巴什/阶梯 Nim |
| `std_compat.hpp` | ISO/GNU 兼容层 |

> 完整函数签名见 [API 速查](API-Reference.md)。
