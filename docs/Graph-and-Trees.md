# 图论与树

[← 首页](Home.md) · [模块全览](Module-Catalog.md)

## 最大流 `std_dinic<T>`

```cpp
std_dinic<long long> dinic(n);   // n 个节点，编号 0..n-1
dinic.add_edge(u, v, cap);       // 有向边 + 反向边
long long f = dinic.max_flow(s, t);
```

容量默认 `long long`。`std_mcmf` 最小费用最大流同理，路径费用要算全（0→1 的边权容易漏）。

## 二分图

```cpp
hopcroft_karp hk(nL, nR);         // 左部点从 0 开始
hk.add(u, v);                     // u∈左, v∈右
int m = hk.max_matching();

km_result r = km_max_weight(w, nL, nR);  // KM 最大权完美匹配
r.match;  r.weight;
```

KM 内部会跳过无边的 `NEG_INF`，无边权建议取 `-(1LL<<50)` 防止顶标调整后 gap 溢出。

## 2-SAT

```cpp
two_sat sat;
sat.reset(n);                     // n 个布尔变量
sat.add(a, va, b, vb);            // 子句 (a==va) ∨ (b==vb)
sat.add_unit(a, val);             // 单元子句
std::vector<char> assignment;
bool ok = sat.satisfiable(&assignment);
```

## Tarjan（割点/桥/连通分量）

```cpp
tarjan_result r = tarjan(g);      // g 是 vector<vector<int>> 邻接表
r.is_cut;                         // 割点标记
r.bridges;                        // 桥
```

## Link-Cut Tree（动态树）

```cpp
link_cut_tree<long long> lct(n);
lct.set_val(x, v);
lct.link(u, v); lct.cut(u, v);
lct.connected(u, v);
lct.makeroot(x);
lct.path_add(u, v, delta);        // 路径加
lct.path_max(u, v);               // 路径最大
```

> `connected` 内部是 `makeroot(x); findroot(y)==x`。

## 树 LCA 与虚树

```cpp
std::vector<std::vector<int>> g = {{1,3},{0,2},{1},{0}};
tree_lca lca((int)g.size(), g);
lca.lca(a, b);                    // 最近公共祖先
lca.dist(a, b);                   // 边数距离
lca.dfn(u);                       // DFS 序

auto vt = virtual_tree(g, lca, {2, 3});   // 关键点压缩
// vt.second: 虚树节点（原图编号），vt.first[i]: (节点下标, 边权) 邻接表
```

> 虚树必须按 **DFS 序** 排序关键点，库内部已处理。

## 树直径 / 重心

```cpp
int u, v;
int d = tree_diameter(g, u, v);           // 返回直径边数，u/v 带出端点
int c = tree_centroid(g);                  // 重心
```

## 圆方树 / 仙人掌

```cpp
block_cut_tree bct(g);            // 点双圆方树
bct.size(); bct.dist(a, b);       // 原图两点在圆方树上的距离
cactus_dist(g, u, v);             // 仙人 cactus 上距离
```

## 网格图

`std_grid` 是基类，`std_grid_adjacency_list` / `Grid_adjacency_matrix` 是两种派生：

```cpp
Grid_adjacency_list g(n);
g.add_edge(u, v, w);              // 公开方法
```

> 邻接矩阵版会自动建无向边；有向测试要用 `set_edge(反向, 0)` 抵消。

## 图算法包 `std_grid_algorithm`

包含 Dijkstra、最小生成树（Prim/Kruskal）、欧拉路/回路、二分图判定、强连通缩点等，接口见头文件注释。
