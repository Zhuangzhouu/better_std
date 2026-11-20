# better_std — 竞赛 C++ 算法库

一个面向算法竞赛的**单头文件 C++ 算法库**：一行 `#include"std_better.hpp"` 引入全部 88 个头文件。

> 命名空间：`better_std`　|　语言基线：**ISO C++11**（C++11/14/17/20/23 与 gnu++ 系列全部编译通过）

## 特点

- **零依赖**：只用 C++ 标准库，不需要 boost 或任何第三方库
- **广泛兼容**：`g++ -std=c++11 -pedantic-errors` 严格 ISO 模式到 `-std=gnu++23` 全部零错误；GNU 模式自动启用 `__int128` / `__builtin_*` 加速
- **多 TU 安全**：所有自由函数 `inline`，头文件自带所需标准头，两个 `.cpp` 同时 include 可正常链接
- **纯头文件**：全模板、全内联，无需编译 `.cpp`、无需链接步骤
- **持续验证**：GitHub Actions 在每次推送时对 10 种编译标准跑编译 + 对拍测试

## 快速开始

```cpp
#include"std_better.hpp"
using namespace better_std;

int main() {
    // 高精度整数（精度作为模板参数）
    High_precision_number<50> a("12345678901234567890"), b(7);
    std::cout << (a * b + b) << "\n";

    // 排序 / 选择
    std::vector<int> v = {5, 3, 9, 1, 7};
    quick_sort(v.begin(), v.end());

    // 字符串：后缀数组
    suffix_array sa("banana");

    // 多项式因式分解
    auto res = factorize("x^2-1");

    return 0;
}
```

编译：

```bash
g++ -std=c++11 -O2 -Wall -Wextra main.cpp -o main
```

> **Windows / MSYS2 注意**：链接时加 `-static`，否则生成的 exe 运行时会因缺少 MSYS2 运行库而以 127 退出。编译前先 `export PATH="/d/msys2/ucrt64/bin:$PATH"`。
>
> **GNU 加速**：把 `-std=c++11` 换成 `-std=gnu++17` 即自动启用 `__int128` / `__builtin_*` 快速路径；注意 GNU 模式不要加 `-pedantic-errors`（`__int128` 会被判为违规）。

## 示例与测试

- [`examples/`](examples/) — 分模块可运行示例，含一键编译脚本
- [`tests/`](tests/) — 对拍测试，含覆盖 10 种标准的一键脚本

```bash
cd examples && bash build.sh     # 编译所有示例
cd tests    && bash run_tests.sh # 10 种标准编译 + 运行对拍
```

## 模块一览

| 类别 | 内容 |
|---|---|
| 数值 | 高精度整数/浮点、分数、复数、矩阵、多项式（NTT/FFT/inv·ln·exp） |
| 容器 | 堆、二叉搜索树、字典树、并查集、树状数组、双端优先队列 |
| 区间 | 线段树（懒标记）、Segment Tree Beats、ST 表、主席树、猫树 |
| 图/网络流 | 网格图、图算法包、Dinic、最小费用最大流、二分图匹配、KM、2-SAT |
| 树 | 无根树 LCA、有根树、二叉树、重链剖分、Link-Cut Tree、虚树、圆方树 |
| 字符串 | KMP/Z/哈希/Manacher、AC 自动机、后缀数组/自动机、回文自动机、后缀树、哈希全家桶 |
| 数论 | modpow/exgcd/CRT/线性同余、线性筛、Lucas、BSGS、原根、Miller-Rabin + Pollard-Rho、杜教筛、BM 线性递推 |
| 几何 | 点/向量、凸包、鞋带面积、线段相交、点在多边形、矩形面积并 |
| DP/优化 | 李超线段树、凸壳斜率优化、单调队列 DP、四边形不等式相关 |
| 分治/离线 | CDQ 分治、整体二分、线段树分治、莫队、分块 |
| 组合计数 | 卡特兰、斯特林、贝尔、划分数、错排、容斥、Burnside/Polya |
| 数值计算 | Simpson 自适应积分、二分/牛顿求根、拉格朗日插值、单纯形法 |
| 3SUM 家族 | two/three/four-sum、计数版、bitset 位压加速判定 |
| 博弈论 | Nim、SG 函数、巴什博弈、阶梯 Nim |
| 重构树/分治 | Kruskal 重构树（瓶颈路）、点分治（路径计数） |
| 几何进阶 | 最小圆覆盖、半平面交 |
| 高级数据结构 | Splay（集合/序列）、可持久化 Trie（区间异或） |
| 莫队变体 | 带修莫队、回滚莫队 |
| 兼容层 | `std_compat.hpp`：ISO 回退 / GNU 扩展自动分流 |

## 编译验证

每个模块都经过与暴力/标准库的对拍验证，覆盖 C++11/14/17/20/23 及对应 GNU 模式共 10 种标准，输出逐字节一致。CI 与本地脚本（`tests/run_tests.sh`）使用同一套测试。

## 目录结构

```
.
├── std_better.hpp      # 唯一入口，按依赖拓扑顺序引入全部模块
├── std_*.hpp           # 88 个算法头文件，位于仓库根目录
├── examples/           # 可运行示例 + build.sh
├── tests/              # 对拍测试 + run_tests.sh
└── .github/workflows/  # CI：10 种标准自动编译 + 测试
```

## 版本

当前版本 **v1.0.0**。变更记录见 [CHANGELOG.md](CHANGELOG.md)。

## License

[MIT](LICENSE)
