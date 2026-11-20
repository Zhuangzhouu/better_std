# 数据结构

[← 首页](Home.md) · [模块全览](Module-Catalog.md)

## 树状数组 `std_fenwick<T>`

```cpp
std_fenwick<long long> bit(n);   // 下标从 1 开始
bit.add(i, v);                   // 单点加：a[i] += v
bit.sum(i);                      // 前缀和：Σ a[1..i]
bit.range_sum(l, r);             // 区间和 [l,r]
int k = bit.kth(k);              // 第 k 小（k 从 1 起），要求元素非负
```

## 线段树 `std_segment_tree<T>`

```cpp
std_segment_tree<long long> seg(n);
seg.build(a, 1, 0, n-1);          // 建树
seg.query(l, r);                  // 区间查询
seg.update(pos, val);             // 单点修改
seg.range_add(l, r, v);           // 区间加（懒标记）
```

`std_segment_tree_beats` 额外支持区间 `chmin` + 区间加，维护最大/最小值。

## 主席树 `std_persistent_segment_tree`

静态区间第 k 小，复用线段树节点，版本化。

## 猫树 `cat_tree`

O(1) 区间查询，支持**非幂等**运算（如组合信息合并）。用按位异或下标访问预计算表，`lg` 表按 `2^K` 大小开。

```cpp
auto add = [](long long x, long long y){ return x + y; };
std::vector<long long> a = {1,2,3,4,5};
cat_tree<long long, decltype(add)> ct(a, add);
ct.query(l, r, add);             // [l,r] 聚合
```

## 二叉搜索树 `std_bst<T>`

```cpp
std_bst<int> bst;
bst.insert(k); bst.erase(k);
bst.contains(k);
bst.kth(k);                      // 第 k 小（1-based，k 从 1 起）
bst.rank(k);                     // 严格小于 k 的个数
bst.count_range(lo, hi);
bst.predecessor(k); bst.successor(k);  // pair<bool,T>
bst.inorder(); bst.min(); bst.max();
```

> 实现用迭代版插入（递归版持 vector 引用会在 realloc 后悬垂）。

## 并查集家族

```cpp
std_dsu dsu(n);                  // find / unite / same
rollback_dsu rd(n);              // 可撤销：unite / undo / same
persistent_dsu pd(n);            // 可持久化
weighted_dsu<Monoid> wd(n);      // 带权（泛型 Monoid）
type_dsu td(n);                  // K 种关系
interval_dsu id(n);              // 带权区间
std_weighted_dsu                 // XOR 别名
```

## 可并堆三件套

```cpp
leftist_heap<int> lh;   // 左偏树
skew_heap<int> sh;      // 斜堆
pairing_heap<int> ph;   // 配对堆
// 共同接口：push / top / pop / merge(其他堆) / empty / size
```

> 三个类各自持有独立 `vector`，`merge`  internally 收集对方所有值再逐个 push（不能直接拿对方根索引）。

## 其他

| 结构 | 说明 |
|---|---|
| `std_heap` | 二叉堆 |
| `std_minmax_heap` | 双端优先队列（同时取最大/最小） |
| `std_trie` | 字典树 |
| `std_linear_basis` / `prefix_linear_basis` | 异或线性基 / 区间最大异或 |
| `sqrt_decomposition` | 分块：区间加、区间和、区间最值、≤x 计数 |
| `std_sparse_table` | RMQ，`O(1)` 查询 |
| `std_persistent_segment_tree` | 主席树 |
| `std_cat_tree` | 猫树，O(1) 非幂等区间查询 |

`fhq_treap`（在 `std_treap.hpp`）是无旋 Treap，支持集合语义 `build_set` 和序列语义 `build`，后者可区间翻转：

```cpp
fhq_treap<> treap;
treap.insert(key);
treap.rank_of(key);
treap.reverse_range(l, r);       // 翻转后树不再按 key 有序，勿再 insert/rank
```
