#pragma once
//===== better_std::std_dsu_extra —— 并查集扩展：可撤销 / 可持久化 =====
//  rollback_dsu      : 按 size 合并、无路径压缩；save()/rollback() 打点回滚，undo() 撤销一步
//  persistent_dsu    : 基于节点式可持久化数组，按 size 合并、无路径压缩；
//                      每次 unite 产生新版本号（O(log n)），支持任意版本查询与 revert
#include <vector>
#include <utility>
#include <algorithm>
#include <cstddef>
namespace better_std {

//==================== 可撤销并查集 ====================
struct rollback_dsu {
    std::vector<int> par, sz;
    std::vector<std::pair<int, int>> hist;   // 记录每次合并 (child, parent)

    rollback_dsu() {}
    explicit rollback_dsu(int n) { init(n); }

    void init(int n) {
        par.resize(n);
        sz.assign(n, 1);
        for (int i = 0; i < n; i++) par[i] = i;
        hist.clear();
    }
    int find(int x) const {
        while (par[x] != x) x = par[x];
        return x;
    }
    bool same(int x, int y) const { return find(x) == find(y); }
    int size_of(int x) const { return sz[find(x)]; }

    // 成功合并返回 true；已同集合返回 false（不产生历史）
    bool unite(int x, int y) {
        x = find(x); y = find(y);
        if (x == y) return false;
        if (sz[x] < sz[y]) std::swap(x, y);
        hist.emplace_back(y, x);
        par[y] = x;
        sz[x] += sz[y];
        return true;
    }
    // 撤销最后一次成功合并（无历史则 no-op）
    void undo() {
        if (hist.empty()) return;
        auto [child, parent] = hist.back();
        hist.pop_back();
        par[child] = child;
        sz[parent] -= sz[child];
    }
    int checkpoint() const { return (int)hist.size(); }
    void rollback(int cp) { while ((int)hist.size() > cp) undo(); }
};

//==================== 可持久化数组（节点式线段树，O(log n) 单点改/查）=====================
struct persistent_array {
    struct node { int lc, rc, val; };
    std::vector<node> tr;
    int n;
    persistent_array() : n(0) {}

    // 用 a 建初始版本，返回根下标
    int build(const std::vector<int>& a) {
        n = (int)a.size();
        tr.clear();
        if (n == 0) return -1;
        return build_rec(0, n - 1, a);
    }
    // 在 root 版本上把 pos 改为 val，返回新版本根（旧版本不变）
    int set(int root, int pos, int val) { return set_rec(root, 0, n - 1, pos, val); }
    // 查询 root 版本中 pos 的值
    int get(int root, int pos) const { return get_rec(root, 0, n - 1, pos); }

private:
    int build_rec(int l, int r, const std::vector<int>& a) {
        int id = (int)tr.size();
        tr.push_back({-1, -1, 0});
        if (l == r) { tr[id].val = a[l]; return id; }
        int m = (l + r) >> 1;
        tr[id].lc = build_rec(l, m, a);
        tr[id].rc = build_rec(m + 1, r, a);
        return id;
    }
    int set_rec(int root, int l, int r, int pos, int val) {
        int id = (int)tr.size();
        tr.push_back(tr[root]);
        if (l == r) { tr[id].val = val; return id; }
        int m = (l + r) >> 1;
        if (pos <= m) tr[id].lc = set_rec(tr[root].lc, l, m, pos, val);
        else tr[id].rc = set_rec(tr[root].rc, m + 1, r, pos, val);
        return id;
    }
    int get_rec(int root, int l, int r, int pos) const {
        while (l < r) {
            int m = (l + r) >> 1;
            if (pos <= m) { root = tr[root].lc; r = m; }
            else { root = tr[root].rc; l = m + 1; }
        }
        return tr[root].val;
    }
};

//==================== 可持久化并查集 ====================
// 版本语义：ver 从 0 开始；unite 在 current 版本上操作并产生新版本（返回新版本号）；
// same/find/size_of 可查任意历史版本；revert(ver) 把 current 移回历史版本（数据保留，可继续分叉）。
struct persistent_dsu {
    persistent_array par, sz;
    int n;
    std::vector<int> ver_par, ver_sz;   // 每个版本的数组根
    int cur;                            // 当前版本号

    persistent_dsu() : n(0), cur(0) {}
    explicit persistent_dsu(int n_) { init(n_); }

    void init(int n_) {
        n = n_;
        std::vector<int> p(n), s(n, 1);
        for (int i = 0; i < n; i++) p[i] = i;
        ver_par.clear(); ver_sz.clear();
        int rp = par.build(p);
        int rs = sz.build(s);
        ver_par.push_back(rp);
        ver_sz.push_back(rs);
        cur = 0;
    }

    // 在指定版本中查询 x 的根（无路径压缩，O(log n) 层级）
    int find(int x, int ver) const {
        int p = par.get(ver_par[ver], x);
        while (p != x) {
            x = p;
            p = par.get(ver_par[ver], x);
        }
        return x;
    }
    bool same(int x, int y, int ver) const { return find(x, ver) == find(y, ver); }
    int size_of(int x, int ver) const {
        int r = find(x, ver);
        return sz.get(ver_sz[ver], r);
    }

    // 在当前版本上合并 x,y，产生并返回新版本号。
    // 版本 nv 恒存于 ver_par[nv]：revert 后再次 unite 会原位覆盖旧分支的同号版本。
    int unite(int x, int y) {
        int rx = find(x, cur), ry = find(y, cur);
        int nv = cur + 1;
        auto set_ver = [&](int rp, int rs) {
            if (nv < (int)ver_par.size()) { ver_par[nv] = rp; ver_sz[nv] = rs; }
            else { ver_par.push_back(rp); ver_sz.push_back(rs); }
            cur = nv;
        };
        if (rx == ry) { set_ver(ver_par[cur], ver_sz[cur]); return nv; }
        int sx = sz.get(ver_sz[cur], rx), sy = sz.get(ver_sz[cur], ry);
        if (sx < sy) std::swap(rx, ry);
        int np = par.set(ver_par[cur], ry, rx);
        int ns = sz.set(ver_sz[cur], rx, sx + sy);
        set_ver(np, ns);
        return nv;
    }
    int snapshot() const { return cur; }
    void revert(int ver) { cur = ver; }
};

} // namespace better_std
