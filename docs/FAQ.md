# 常见问题

[← 首页](Home.md)

## 为什么不用 boost / bits/stdc++.h？

CCF 的 NOI 系列只承诺提供 ISO C++ 标准库 + g++，boost 是第三方库、不保证版本，`bits/stdc++.h` 是 GCC 扩展。这个库刻意只用标准库头文件，评测机装不装 boost 都无所谓。`bits/stdc++.h` 跟编译器绑死可用，但这里为了 ISO 合规选择了标准头。

## 下标约定（重要）

不同模块的约定不统一，首次使用前务必确认：

| 模块 | 约定 |
|---|---|
| `std_fenwick` | 下标 **1-based** |
| `std_segment_tree` | 一般 1-based（节点编号从 1） |
| `wavelet_tree::kth(l,r,k)` | **0-based 闭区间** `[l,r]`，k 从 0 起 |
| `ob_query`（整体二分） | l/r 为 0-based 闭区间，内部 +1 转 1-based |
| `linear_rec_nth(init,rec,n)` | n 为 **0-based** |
| `std_dinic` / `tree_lca` | 节点编号 0-based |
| 主席树第 k 小 | k 从 1 起 |
| `std_bst::kth(k)` | k 从 **1** 起；`rank(k)` 是严格小于 k 的个数 |

## 编译相关

**Q：gnu++ 模式加 `-pedantic-errors` 报错 `__int128`？**
A：这是预期行为——GNU 扩展本就不是 ISO 标准。GNU 模式不要加 `-pedantic-errors`；ISO 模式才加。

**Q：Windows 下 exe 运行报 127？**
A：链接加了动态依赖 MSYS2 运行库。编译时加 `-static`。

**Q：MSYS2 编译静默失败？**
A：先 `export PATH="/d/msys2/ucrt64/bin:$PATH"`，否则 cc1plus.exe 找不到 DLL。

**Q：两个 .cpp 都 include 全库，链接重复定义？**
A：不应该。所有自由函数都是 `inline`。若遇到，检查自己写的代码是否漏了 `inline`。

## 已知坑点（库已规避，贡献代码时注意）

1. **求值顺序 UB**：`tr[id].l = build_(...)` 这类写法中，右侧递归 `push_back` 致 vector 重分配、左值地址可能先取——C++11/14 与 C++17 表现不同。务必先算局部变量再赋值。
2. **C++11 聚合初始化**：带默认成员初始化器的 struct 不是聚合类，`Node{v,...}` 会编译失败，要显式构造函数。
3. **`std::` 限定**：库内有高精度 `fabs/max/min/to_string` 重载会遮蔽全局函数，内部调用必须写 `std::fabs` 等。
4. **虚树 / DFS 序**：虚树必须按 DFS 序（不是 BFS 序）排序关键点。
5. **可并堆 merge**：各堆持有独立 vector，不能用对方根索引直接 merge。
6. **sg_dag**：要求"后继编号 < u"（拓扑序反向），正向编号会算错。
7. **gooddouble 很慢**：迭代每步要 `to_decimals(accuracy)` 封顶，否则 div_number 膨胀卡死；40 万次 CHECK 可能要几小时，测试用 accuracy=12。
8. **FHQ Treap**：`build` 是序列语义、`build_set` 是集合语义；区间翻转后树不再按 key 有序，勿再 insert/rank。
9. **rolling_hash**：`get(l,len)` 已是规范化哈希，勿再乘 `base^{-(n-l-len)}`。
10. **CAT 树**：`lg` 表按 `2^K` 大小开（查询下标是 `l^r`，可能远大于 n）。
11. **`std_bst` C++11 两处坑**：`Node` 带默认成员初始化器时不是聚合类，`push_back({k,...})` 会编译失败（已加显式构造函数）；`pool[u].l=new_node(k)` 右侧 push_back 重分配致求值顺序 UB（已改成先算 nid 再赋值）。

完整坑点清单见仓库 `.workbuddy/memory/MEMORY.md`（本地记忆，不上库）。

## 高精度用法

```cpp
High_precision_number<50> a("12345678901234567890");  // 字符串构造
High_precision_number<50> b(7);                        // 整数构造
std::cout << (a * b + b);                              // 流输出
```

- 字符串构造**不支持科学计数法**（`"1e3"` 会被当成 13）。
- 混合运算**故意排除 unsigned**（防截断），用 `to_long_long()` 全程 signed。
- `divmod` 余数符号同被除数；`powmod` 负底数先归一化。
- `std::numeric_limits` 的 max/min 返回 0 仅作编译占位，DP 要显式传 INF。

## 为什么头文件都在根目录？

方便一行 `#include"std_better.hpp"` 引入（其 include 路径就是同级目录）。`std_better.hpp` 按依赖拓扑顺序引入全部模块。子目录只用于 `examples/`、`tests/`、`docs/` 和 CI 配置。
