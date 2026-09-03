#pragma once
//===== better_std::weighted_dsu —— 带权（种类）并查集 =====
//通用加权并查集：每个节点维护相对父节点的"权值 delta"，
//通过 Monoid trait 描述权值的单位元/组合/求逆，从而支持带关系约束的合并。
//
//  经典应用：种类并查集（食物链 / 奇偶关系）—— 用 XOR 或模加 Monoid 即可。
//
//  Monoid 约定（静态成员）：
//    using T = ...;            // 权值类型
//    static T id();            // 单位元
//    static T op(T a, T b);    // 组合（a 接 b）
//    static T inv(T a);        // 求逆（a 的逆）
namespace better_std {

//===== 泛型加权并查集 =====
template<typename Monoid>
struct weighted_dsu {
    using T = typename Monoid::T;
    std::vector<int> parent;
    std::vector<int> rank;
    std::vector<T>   weight;   // weight[x] = x 相对 parent[x] 的权值

    explicit weighted_dsu(int n) : parent(n), rank(n, 0), weight(n, Monoid::id()) {
        for (int i = 0; i < n; ++i) parent[i] = i;
    }

    // 返回 x 的根，并路径压缩（同时把 weight 更新为相对根的真实权值）
    int find(int x) {
        if (parent[x] == x) return x;
        int root = find(parent[x]);
        // 由 "x -> parent[x] -> ... -> root" 推出 x 相对 root 的权值
        weight[x] = Monoid::op(weight[x], weight[parent[x]]);
        parent[x] = root;
        return root;
    }

    // 查询 x 相对其根的权值（不保证路径压缩到根，先 find 一下）
    T rel_to_root(int x) {
        find(x);
        return weight[x];
    }

    // 查询 a 相对 b 的权值：rel(a) - rel(b)，即 rel(a) 接 rel(b)的逆
    // 仅当 a,b 同属一个连通块时有意义
    T diff(int a, int b) {
        find(a); find(b);
        // weight[x] 是相对根的权值，a 相对 b = op(rel(a), inv(rel(b)))
        return Monoid::op(weight[a], Monoid::inv(weight[b]));
    }

    // 合并：令 a 所在集合相对 b 所在集合的偏移为 w
    // 即要求 rel(a) = op(rel(b), w)（a 比 b "多" w）
    // 返回 false 表示与已有关系矛盾（同根且计算出不一致）
    bool unite(int a, int b, T w) {
        int ra = find(a), rb = find(b);
        if (ra == rb) {
            // 同根：校验一致性  diff(a,b) 应等于 w
            return Monoid::op(weight[a], Monoid::inv(weight[b])) == w;
        }
        // 约定把秩小的挂到秩大的下面，保持平衡
        if (rank[ra] < rank[rb]) {
            std::swap(ra, rb);
            std::swap(a, b);
            w = Monoid::inv(w);   // 交换 a,b 后偏移取逆
        }
        // 现 ra 为新根。需要 weight[rb] 使得:
        //   rel(a) = op(rel(b), w) 且 rel(a) = op(rel(ra)=id?, ...)
        // 推导：rel(a) = op(weight[a], rel(ra)) = weight[a]（ra 是根）
        //       rel(b) = op(weight[b], rel(rb))
        // 令 root(rb)=ra，则 rel(b) = op(weight[b], weight[rb])
        // 代入 rel(a)=op(rel(b),w):
        //   weight[a] = op( op(weight[b], weight[rb]), w )
        // => weight[rb] = op( inv(weight[b]), op( inv(w), weight[a] ) )
        T wrb = Monoid::op(Monoid::inv(weight[b]),
                           Monoid::op(Monoid::inv(w), weight[a]));
        parent[rb] = ra;
        weight[rb] = wrb;
        if (rank[ra] == rank[rb]) ++rank[ra];
        return true;
    }

    bool same(int a, int b) { return find(a) == find(b); }
};

//===== 模加 Monoid（模 mod 加法群） =====
template<int MOD>
struct add_mod_monoid {
    using T = int;
    static T id()   { return 0; }
    static T op(T a, T b) { return (a + b) % MOD; }
    static T inv(T a)     { return (MOD - a % MOD) % MOD; }
};

//===== XOR Monoid（0/1 奇偶 / 种类并查集经典） =====
struct xor_monoid {
    using T = int;
    static T id()   { return 0; }
    static T op(T a, T b) { return a ^ b; }
    static T inv(T a)     { return a; }   // 自逆
};

// 常用别名：XOR 种类并查集
using parity_dsu = weighted_dsu<xor_monoid>;

} // namespace better_std
