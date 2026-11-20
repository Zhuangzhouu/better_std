#ifndef BETTER_STD_STD_BETTER_HPP
#define BETTER_STD_STD_BETTER_HPP

//===== better_std 数值库总入口：一行 include 全部 =====
//分组按依赖拓扑排列；每个头文件自带 ISO include guard
//兼容层：严格 ISO 模式走纯标准实现，-std=gnu++XX 自动启用 GNU 扩展加速
#include"std_compat.hpp"
//
//  数值：高精度整数/浮点、分数、复数、矩阵、多项式、多项式因式分解
//  容器：堆、二叉搜索树、树状数组、线段树、并查集、字典树
//  字符串：KMP/Z/哈希、AC 自动机
//  图与网络流：Dinic、最小费用最大流、网格图×2、图算法包
//  树：无根树、有根树、二叉树、重链剖分
//  算法工具：数论、单调栈/队列、序列 DP(LIS/LCS)、前缀和

//—— 数值 ——
#include"std_goodint.hpp"
#include"std_gooddouble.hpp"
#include"std_fraction.hpp"
#include"std_complex.hpp"
#include"std_matrix.hpp"
#include"std_polynomial.hpp"
#include"std_poly_factor.hpp"

//—— 容器 ——
#include"std_heap.hpp"
#include"std_bst.hpp"
#include"std_fenwick.hpp"
#include"std_sparse_table.hpp"
#include"std_segment_tree.hpp"
#include"std_segment_tree_beats.hpp"
#include"std_persistent_segment_tree.hpp"
#include"std_dsu.hpp"
#include"std_trie.hpp"

//—— 字符串 ——
#include"std_string_algorithm.hpp"
#include"std_ac_automaton.hpp"

//—— 图与网络流 ——
#include"std_dinic.hpp"
#include"std_mcmf.hpp"
#include"std_grid.hpp"
#include"std_grid_adjacencyList.hpp"
#include"std_grid_adjacency_matrix.hpp"
#include"std_grid_algorithm.hpp"

//—— 树 ——
#include"std_rootless_tree.hpp"
#include"std_tree.hpp"
#include"std_binary_tree.hpp"
#include"std_heavy_light.hpp"

//—— 算法工具：数论 / 单调结构 / 序列 DP / 前缀和 / 排列 / 差分 / 双端堆 / CDQ / 种类并查集 / DP优化 / 后缀数组自动机 / 二分图匹配 / 排序 / 二分三分搜索 / 极值结构 / 并查集扩展 / 计算几何 / 字符串扩展 ——
#include"std_number_theory.hpp"
#include"std_monotonic.hpp"
#include"std_sequence.hpp"
#include"std_prefix_sum.hpp"
#include"std_permutation.hpp"
#include"std_difference.hpp"
#include"std_minmax_heap.hpp"
#include"std_tarjan.hpp"
#include"std_gaussian.hpp"
#include"std_knapsack.hpp"
#include"std_weighted_dsu.hpp"
#include"std_mo.hpp"
#include"std_discretize.hpp"
#include"std_digit_dp.hpp"
#include"std_cdq.hpp"
#include"std_type_dsu.hpp"
#include"std_dp_opt.hpp"
#include"std_sa_sam.hpp"
#include"std_bipartite_matching.hpp"
#include"std_sort.hpp"
#include"std_binary_search.hpp"
#include"std_extrema.hpp"
#include"std_dsu_extra.hpp"
#include"std_geometry.hpp"
#include"std_string_extra.hpp"

//—— 平衡树 / 线性基 / 组合计数 / 最大权匹配 / 分块 ——
#include"std_treap.hpp"
#include"std_linear_basis.hpp"
#include"std_combinatorics.hpp"
#include"std_km.hpp"
#include"std_sqrt_decomposition.hpp"

//—— 猫树（Disjoint Sparse Table，O(1) 区间查询，支持非幂等运算）——
#include"std_cat_tree.hpp"

//—— 高级算法（纯增量追加）：大数分解 / 动态树 / 2-SAT / 杜教筛 / 矩阵树 / Polya / 单纯形 / 多点求值 ——
#include"std_prime_factor.hpp"
#include"std_lct.hpp"
#include"std_two_sat.hpp"
#include"std_dujiao.hpp"
#include"std_matrix_tree.hpp"
#include"std_polya.hpp"
#include"std_simplex.hpp"
#include"std_multipoint.hpp"

//—— 高级算法第二批（纯增量）：FWT/FST / 线性递推 / 可并堆 / 数值 / KD-Tree / 离线 / 哈希 ——
#include"std_fwt.hpp"
#include"std_linear_recurrence.hpp"
#include"std_mergeable_heap.hpp"
#include"std_numerical.hpp"
#include"std_kd_tree.hpp"
#include"std_offline.hpp"
#include"std_hashing.hpp"

//—— "GPT 版" 高效 3SUM 家族（2/3/4SUM + bitset 位压加速判定）——
#include"std_three_sum.hpp"

//—— 高级算法第三批（纯增量）：CRT / 小波树 / 树扩展 / 圆方树 / 博弈论 ——
#include"std_crt.hpp"
#include"std_wavelet_tree.hpp"
#include"std_tree_extra.hpp"
#include"std_block_cut_tree.hpp"
#include"std_game_theory.hpp"
#include"std_kruskal_tree.hpp"
#include"std_centroid_decomposition.hpp"
#include"std_geometry_extra.hpp"
#include"std_splay.hpp"
#include"std_persistent_trie.hpp"
#include"std_mo_variants.hpp"

#endif // BETTER_STD_STD_BETTER_HPP
