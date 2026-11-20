# 变更记录

本项目遵循语义化版本。当前版本 **v1.0.0**。

## v1.0.0 — 首个正式版（2026-11-20）

首个稳定发布。88 个头文件，一行 `#include"std_better.hpp"` 引入全部。

### 库内容
- **数值**：高精度整数/浮点、分数、复数、矩阵（Bareiss 行列式）、多项式（NTT/FFT/inv·ln·exp）、素数判定与 Pollard-Rho 分解、CRT、杜教筛、BM 线性递推、多点求值。
- **容器/区间**：堆、BST、可并堆三件套（左偏树/斜堆/配对堆）、可撤销与持久化并查集、Splay、线性基、线段树（含 Beats）、主席树、猫树、小波树、KD-Tree。
- **图/树**：网格图、Dinic、MCMF、二分图匹配、KM、2-SAT、Tarjan、LCA、重链剖分、Link-Cut Tree、虚树、圆方树、Kruskal 重构树、点分治。
- **字符串**：KMP/Z/Manacher、AC 自动机、后缀数组/自动机、回文自动机、后缀树、滚动哈希全家桶、可持久化 Trie。
- **数论/组合**：线性筛、Lucas、BSGS、原根、组合计数、Burnside/Polya、矩阵树定理。
- **算法工具**：CDQ 分治、整体二分、线段树分治、莫队（含带修/回滚变体）、背包、数位 DP、斜率优化、单纯形法、3SUM 家族、博弈论、模拟计算。
- **几何**：凸包、矩形面积并、最小圆覆盖、半平面交。
- **兼容层**：`std_compat.hpp` 收敛全部 GNU 扩展，ISO C++11 到 GNU C++23 双模式自动分流。

### 质量保障
- **全标准验证**：10 种编译标准（ISO/GNU × C++11/14/17/20/23）逐一编译并运行同一套对拍测试，输出逐字节一致。
- **800 项回归测试**：`tests/test_all.cpp`，含随机对拍（3SUM、小波树、整体二分、点分治、莫队等）。
- **CI**：GitHub Actions 在每次 push/PR 时跑 10 标准矩阵 + 全部示例 + 头文件独立包含检查。
- **文档**：12 页项目 Wiki（`docs/`）、5 个可运行示例（`examples/`）、FAQ 与贡献指南。
- **工程规范**：ISO include guard、自由函数全 `inline`、头文件自包含、多 TU 安全、MIT 许可证。

### 修复记录（本轮）
- `std_bst`：C++11 聚合初始化与求值顺序 UB 两处（插 5 个节点只剩 2 个）。
- `std_splay`：rotate 未更新根指针、erase 合并子树逻辑错误。
- `std_persistent_trie`：min_xor 走另一分支时未置位。
- `std_geometry_extra`：半平面交输出顶点下标环形处理错误。
- `std_mo_variants`：带修莫队 apply_modify 语义重复计数。

## 历史
- v0.x：增量开发期（82 个头文件，GitHub 提交 `6b6aca6` 及之前）。项目最初于 2026-11-20 推送至 <https://github.com/Zhuangzhouu/better_std>。
