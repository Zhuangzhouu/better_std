# 编译与兼容

[← 首页](Home.md)

## 双模式设计

所有 GNU 扩展都被收敛到 `std_compat.hpp`，并用 `#if` 守卫：

| 模式 | 触发 | 行为 |
|---|---|---|
| **ISO 模式** | `-std=c++11/14/17/20/23`（`__STRICT_ANSI__` 定义） | 纯标准回退：加法倍增模乘、`compat::ilog2_32` / `popcount64` |
| **GNU 模式** | `-std=gnu++11/14/17/20/23` | 自动启用 `__int128` / `__builtin_*` 快速路径 |

可用宏查询当前模式：

```cpp
#include"std_compat.hpp"
// BETTER_STD_HAS_INT128      —— 是否可用 __int128
// BETTER_STD_HAS_GNU_BUILTIN —— 是否可用 __builtin_* 内建函数
```

**一个高频坑**：GNU 模式**不要加 `-pedantic-errors`**，否则 `__int128` 会被判为违规扩展而编译失败。ISO 模式加 `-pedantic-errors` 则是安全的（也是 CI 用来守护标准合规的手段）。

## 验证矩阵

项目在 10 种标准下逐一编译并运行同一套对拍测试（775 项），输出逐字节一致：

```
c++11 c++14 c++17 c++20 c++23        （加 -pedantic-errors）
gnu++11 gnu++14 gnu++17 gnu++20 gnu++23
```

本地一键复现：

```bash
cd tests && bash run_tests.sh
```

CI（`.github/workflows/ci.yml`）在每次 push / PR 时自动跑同样的矩阵，外加全部示例编译和 82 个头文件独立包含检查。

## Windows / MSYS2

本项目在 Windows 上用 MSYS2 UCRT64 工具链开发验证（g++ 16.1.0）：

```bash
export PATH="/d/msys2/ucrt64/bin:$PATH"     # 否则 cc1plus.exe 找不到 DLL 会静默失败
g++ -std=c++11 -static main.cpp -o main      # 必须 -static，否则 exe 运行报 127
```

> `-static` 只影响**运行**，不影响编译正确性；Linux 下不需要。

## 多 TU（多个 .cpp）安全

库满足 ODR：

- 所有自由函数 / 全局变量都是 `inline`；
- 每个头文件自带所需标准头，不依赖包含顺序；
- 用 ISO include guard（`BETTER_STD_XXX_HPP`），不用 `#pragma once`。

因此两个 `.cpp` 同时 `#include"std_better.hpp"` 再一起链接不会重复定义：

```bash
g++ -std=c++11 -c a.cpp -o a.o
g++ -std=c++11 -c b.cpp -o b.o
g++ a.o b.o -o app
```

## C++11 基线：不能用的写法

库内部刻意避免了 C++17 特性，若你贡献代码也要遵守：

| 禁用 | 替代 |
|---|---|
| `inline` 变量 | `#if __cplusplus>=201703L` 分支：C++17 用 inline，旧标准用 static |
| 结构化绑定 `auto [a,b]` | 改 `pair.first` / `pair.second` |
| 泛型 lambda `[](auto x)` | 显式类型参数，或 `std::function` |
| 数字分隔符 `5'000'000` | `5000000` |
| `static const int` 传给 `std::min` 等 | 用 `enum { LIM = ... }` 规避 ODR-use |

另外，C++11 中带默认成员初始化器的 struct **不是聚合类**，不能用 `Node{v, -1}` 初始化，要显式写构造函数。

## 不与 std 冲突

库不向 `namespace std` 添加 `abs` / `div` / `gcd` 等重载（这是未定义行为）。大整数版本的 `gcd` / `lcm` / `abs` / `to_string` 放在 `better_std` 下，通过 ADL 解析。

> 因为库内有高精度 `fabs` / `max` / `min` 等重载，库实现内部调用标准数学函数时必须写 `std::fabs` / `std::max` 显式限定——贡献代码时注意。
