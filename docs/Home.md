# better_std Wiki

`better_std` 是一个面向算法竞赛的**单头文件 C++ 算法库**：一行 `#include"std_better.hpp"` 引入全部 88 个头文件。

> 命名空间 `better_std` · 语言基线 **ISO C++11** · 零第三方依赖 · 10 种编译标准持续验证

## 导航

| 页面 | 内容 |
|---|---|
| [快速上手](Quick-Start.md) | 第一个程序、编译命令、常见用法 |
| [编译与兼容](Build-Guide.md) | ISO / GNU 双模式、Windows MSYS2、多 TU、避坑 |
| [模块全览](Module-Catalog.md) | 88 个头文件按类别索引 |
| [数据结构](Data-Structures.md) | 堆、BST、并查集、树状数组、线段树、主席树、猫树等 |
| [图论与树](Graph-and-Trees.md) | 图算法、网络流、匹配、LCA/LCT、虚树、圆方树、2-SAT |
| [字符串与数论](String-and-NumberTheory.md) | KMP/Z/Manacher、AC 自动机、SA/SAM、哈希、数论函数、CRT、Pollard-Rho |
| [高级算法](Advanced.md) | 多项式、FWT、线性递推、数值计算、KD-Tree、离线、3SUM、博弈 |
| [API 速查](API-Reference.md) | 按类别列出高频函数/类签名 |
| [示例集](Examples.md) | `examples/` 下 5 个可运行示例的讲解 |
| [常见问题](FAQ.md) | 设计取舍、编译报错、已知坑点 |
| [贡献指南](Contributing.md) | 如何新增模块、验证流程、代码风格 |

## 设计目标

1. **零依赖**：只用 C++ 标准库，不需要 boost 或任何第三方库——评测机装不装 boost 都无所谓。
2. ** widest compatibility**：从 `-std=c++11 -pedantic-errors` 严格 ISO 到 `-std=gnu++23` 全部零错误，GNU 模式自动启用 `__int128` / `__builtin_*` 加速。
3. **多 TU 安全**：所有自由函数 `inline`，头文件自带所需标准头，两个 `.cpp` 同时 include 可正常链接。
4. **纯头文件**：全模板、全内联，无需编译 `.cpp`、无需链接步骤。
5. **经过对拍**：每个模块都与暴力/标准库对过，CI 在每次 push 时重跑。

## 一句话用法

```cpp
#include"std_better.hpp"
using namespace better_std;
```

详细见 [快速上手](Quick-Start.md)。仓库地址：<https://github.com/Zhuangzhouu/better_std>
