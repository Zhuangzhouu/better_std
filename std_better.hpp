#pragma once
//===== better_std 数值库总入口：一行 include 全部 =====
//分组按依赖拓扑排列；每个头文件自带 #pragma once
//
//  数值：高精度整数/浮点、分数、复数、矩阵、多项式
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
