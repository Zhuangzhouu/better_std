# 快速上手

[← 首页](Home.md)

## 引入

所有头文件在仓库根目录，`std_better.hpp` 是唯一入口：

```cpp
#include"std_better.hpp"
using namespace better_std;
```

需要单独用某个模块也可以直接包含对应头文件（每个头文件自带所需标准头、可独立编译）：

```cpp
#include"std_dinic.hpp"
```

## 第一个程序

```cpp
#include"std_better.hpp"
#include<cstdio>
using namespace better_std;

int main() {
    // 高精度整数：精度是模板参数
    High_precision_number<50> a("12345678901234567890"), b(7);
    std::cout << (a * b + b) << "\n";

    // 排序
    std::vector<int> v = {5, 3, 9, 1, 7};
    quick_sort(v.begin(), v.end());

    // 后缀数组
    suffix_array sa("banana");

    // 多项式因式分解
    std::cout << factorize("x^2-1") << "\n";
    return 0;
}
```

编译：

```bash
g++ -std=c++11 -O2 -Wall -Wextra main.cpp -o main
```

## 典型用法速览

### 高频函数

```cpp
modpow(2, 10, 1000000007);                    // 快速幂 → 1024
first_true([](long long x){return x*x>=50;}, 0, 20);  // 二分第一个满足 → 8
linear_sieve(100);                            // 线性筛
crt_general({2,3}, {3,5});                   // CRT → 8 (mod 15)
is_prime64(600851475143ULL);                  // Miller-Rabin
prime_factors(600851475143ULL);               // Pollard-Rho 分解
knapsack_01(w, v, W);                         // 0-1 背包
lis(a);                                       // 最长上升子序列长度
convex_hull(points);                          // 凸包
```

### 高频数据结构

```cpp
std_fenwick<long long> bit(n);       // 树状数组：add(i,v), sum(i), kth(k)
std_segment_tree<T> seg(n);          // 线段树：build, query, update
std_dinic<long long> dinic(n);       // 最大流：add_edge, max_flow(s,t)
two_sat sat; sat.reset(n); sat.add(a,va,b,vb); sat.satisfiable();
tree_lca lca(n, g);                  // LCA：lca(a,b), dist(a,b)
link_cut_tree<long long> lct(n);     // 动态树：link, cut, path_max
suffix_array sa("banana");           // sa.sa / sa.rk / sa.lcp
wavelet_tree<int> wt(a);             // wt.kth(l,r,k) 闭区间
```

> ⚠️ 各函数的**下标约定**（0-based / 1-based、闭区间 / 半开）不完全统一，详见 [API 速查](API-Reference.md) 和 [常见问题](FAQ.md)，首次使用前务必确认。

## 编译参数速查

| 场景 | 命令 |
|---|---|
| 比赛提交（最稳） | `g++ -std=c++11 -O2 main.cpp` |
| 严格 ISO 校验 | `g++ -std=c++17 -pedantic-errors -Wall -Wextra main.cpp` |
| GNU 加速 | `g++ -std=gnu++17 -O2 main.cpp` |
| 多文件项目 | 每个 TU 都 `#include"std_better.hpp"`，正常一起编译即可 |

Windows / MSYS2 的 `-static` 等注意事项见 [编译与兼容](Build-Guide.md)。

## 更多示例

仓库 `examples/` 下有 5 个完整可运行示例，见 [示例集](Examples.md)。
