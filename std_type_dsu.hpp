#pragma once
//===== better_std::std_type_dsu —— 种类并查集延伸 + 带权区间 DSU =====
//  1) type_dsu<K> : K 种并查集（扩展 parity_dsu 的 2 种到任意 K 种关系）
//     维护每个节点相对根的「种类偏移」(mod K)，支持：
//       unite(x,y,delta) : 声明 type(y)-type(x) ≡ delta (mod K)，返回是否与已有关系矛盾
//       rel(x,y)         : 返回 type(y)-type(x) (mod K)，不连通返回 -1
//       same(x,y)
//     经典应用：食物链(K=3)、带权种类关系一致性判定。
//  2) interval_dsu : 带权区间 DSU（并查集优化区间覆盖/染色）
//     用「下一个未覆盖位置」并查集，O(α) 摊还地遍历 [l,r] 内尚未覆盖的每个下标，
//     配合回调 f(pos) 给每个位置赋权/染色；常用于区间赋值、区间去重等问题。
#include<bits/stdc++.h>
namespace better_std {

//—— K 种并查集 ——
template<int K>
struct type_dsu {
    static_assert(K >= 1, "K must be >= 1");
    std::vector<int> par, rnk, d;   // d[x] = type(x) - type(par[x])  (mod K)
    explicit type_dsu(int n) : par(n), rnk(n, 0), d(n, 0) {
        std::iota(par.begin(), par.end(), 0);
    }
    int find(int x) {
        if (par[x] == x) return x;
        int p = par[x];                 // 先捕获旧父，避免递归后 par[x] 已指向根
        int r = find(p);
        d[x] = (d[x] + d[p]) % K;
        if (d[x] < 0) d[x] += K;
        par[x] = r;
        return r;
    }
    bool same(int x, int y) { return find(x) == find(y); }
    // 返回 type(y)-type(x) mod K；不连通返回 -1
    int rel(int x, int y) {
        if (!same(x, y)) return -1;
        int r = (d[y] - d[x]) % K;
        if (r < 0) r += K;
        return r;
    }
    // 设定 type(y) - type(x) ≡ delta (mod K)。返回是否与已有关系相容。
    bool unite(int x, int y, int delta) {
        int rx = find(x), ry = find(y);
        delta = ((delta % K) + K) % K;
        if (rx == ry) {
            int cur = (d[y] - d[x]) % K;
            if (cur < 0) cur += K;
            return cur == delta;       // 已连通：检验一致性
        }
        // d[ry] = type(ry)-type(rx) = (d[x]-d[y]+delta) mod K
        int v = (d[x] - d[y] + delta) % K;
        if (v < 0) v += K;
        if (rnk[rx] >= rnk[ry]) {
            par[ry] = rx; d[ry] = v;
            if (rnk[rx] == rnk[ry]) rnk[rx]++;
        } else {
            par[rx] = ry; d[rx] = (K - v) % K;   // type(rx)-type(ry) = -v
        }
        return true;
    }
};

//—— 带权区间 DSU ——
struct interval_dsu {
    int n;
    std::vector<int> nxt;   // nxt[i] = 下一个未覆盖下标(>=i)；nxt[n]=n 哨兵
    explicit interval_dsu(int n_) : n(n_), nxt(n_ + 1) {
        std::iota(nxt.begin(), nxt.end(), 0);
    }
    // 返回 >= i 的第一个未覆盖下标（n 表示后面都没有）
    int find(int i) {
        if (i >= n) return n;
        int r = i;
        while (nxt[r] != r) r = nxt[r];
        for (int cur = i; cur != r; ) {     // 路径压缩
            int nx = nxt[cur];
            nxt[cur] = r;
            cur = nx;
        }
        return r;
    }
    // i 是否已被覆盖
    bool covered(int i) { return i >= 0 && i < n && find(i) != i; }
    // 对 [l,r] 内「尚未覆盖」的每个位置调用 f(pos) 并标记覆盖。返回实际覆盖个数。
    template<typename F>
    int cover(int l, int r, F f) {
        l = std::max(l, 0);
        r = std::min(r, n - 1);
        if (l > r) return 0;
        int cnt = 0;
        for (int i = find(l); i <= r; i = find(i + 1)) {
            f(i);
            nxt[i] = find(i + 1);   // 标记 i 已覆盖，跳到下一个未覆盖
            cnt++;
        }
        return cnt;
    }
};

} // namespace better_std
