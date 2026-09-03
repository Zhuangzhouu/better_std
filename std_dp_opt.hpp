#pragma once
//===== better_std::std_dp_opt —— 斜率 / 单调队列优化 DP =====
//  1) li_chao_tree  : 动态李超线段树，可任意顺序加入直线 y=kx+b，查询某 x 处最值（min/max）。
//  2) convex_hull_trick_min / _max : 单调斜率优化（斜率单调递减/递增、查询 x 单调递增）。
//  3) sliding_window_min_dp : 单调队列优化 DP 模板
//        dp[i] = g[i] + min_{j∈[i-R,i-L]} (dp[j]+w[j])，O(n) 滑动窗口最小值。
#include<bits/stdc++.h>
namespace better_std {

//—— 李超线段树 ——
struct li_chao_tree {
    struct line { long long k, b; long long eval(long long x) const { return k * x + b; } };
    struct node { line seg; node *l, *r; node(line s) : seg(s), l(nullptr), r(nullptr) {} };
    long long XMIN, XMAX;
    node* root;
    bool is_min;   // true 求 min，false 求 max
    li_chao_tree(long long xmin, long long xmax, bool minimize = true)
        : XMIN(xmin), XMAX(xmax), root(nullptr), is_min(minimize) {}
    ~li_chao_tree() { destroy(root); }
    void destroy(node* t) { if (!t) return; destroy(t->l); destroy(t->r); delete t; }

    // a 是否比 b「更优」
    bool better(line a, line b, long long x) const {
        if (is_min) return a.eval(x) < b.eval(x);
        return a.eval(x) > b.eval(x);
    }
    void add_line(long long k, long long b) {
        line s{k, b};
        root = insert(root, s, XMIN, XMAX);
    }
    long long query(long long x) const {
        long long v = query(root, x, XMIN, XMAX);
        return v;
    }
private:
    node* insert(node* t, line nw, long long l, long long r) {
        if (!t) return new node(nw);
        long long m = l + (r - l) / 2;
        bool at_mid = better(nw, t->seg, m);
        if (at_mid) std::swap(t->seg, nw);          // 中点更优者留在节点
        if (l == r) return t;
        bool at_left = better(nw, t->seg, l);
        if (at_left == at_mid) {                     // 较差者在左半更优 -> 进左
            if (at_left) t->l = insert(t->l, nw, l, m);
            else t->r = insert(t->r, nw, m + 1, r);
        } else {
            if (at_left) t->l = insert(t->l, nw, l, m);
            else t->r = insert(t->r, nw, m + 1, r);
        }
        return t;
    }
    long long query(node* t, long long x, long long l, long long r) const {
        if (!t) return is_min ? std::numeric_limits<long long>::max()
                              : std::numeric_limits<long long>::min();
        long long best = t->seg.eval(x);
        long long m = l + (r - l) / 2;
        long long v;
        if (x <= m) v = query(t->l, x, l, m);
        else v = query(t->r, x, m + 1, r);
        return is_min ? std::min(best, v) : std::max(best, v);
    }
};

//—— 单调凸壳技巧（斜率优化）——
struct convex_hull_trick_min {
    struct line { long long k, b; long long eval(long long x) const { return k * x + b; } };
    std::deque<line> dq;
    // 加入直线 y=kx+b，要求斜率 k 单调递减（查询 x 单调递增）。
    void add(long long k, long long b) {
        line nl{k, b};
        if (!dq.empty() && dq.back().k == nl.k) {       // 同斜率只保留更优者
            if (nl.b <= dq.back().b) dq.pop_back(); else return;
        }
        while (dq.size() >= 2) {
            line a = dq[dq.size() - 2], bb = dq.back();
            double x1 = cross(a, bb), x2 = cross(bb, nl);
            if (x1 >= x2) dq.pop_back(); else break;   // bb 无用
        }
        dq.push_back(nl);
    }
    // 查询 x 处最小值（x 须单调递增）
    long long query(long long x) {
        while (dq.size() >= 2 && dq[0].eval(x) >= dq[1].eval(x)) dq.pop_front();
        return dq.front().eval(x);
    }
    static double cross(line a, line b) { return (double)(b.b - a.b) / (double)(a.k - b.k); }
};

struct convex_hull_trick_max {
    struct line { long long k, b; long long eval(long long x) const { return k * x + b; } };
    std::deque<line> dq;
    // 加入直线 y=kx+b，要求斜率 k 单调递增（查询 x 单调递增）。
    void add(long long k, long long b) {
        line nl{k, b};
        if (!dq.empty() && dq.back().k == nl.k) {       // 同斜率只保留更优者
            if (nl.b >= dq.back().b) dq.pop_back(); else return;
        }
        while (dq.size() >= 2) {
            line a = dq[dq.size() - 2], bb = dq.back();
            double x1 = cross(a, bb), x2 = cross(bb, nl);
            if (x1 <= x2) dq.pop_back(); else break;   // bb 无用
        }
        dq.push_back(nl);
    }
    long long query(long long x) {
        while (dq.size() >= 2 && dq[0].eval(x) <= dq[1].eval(x)) dq.pop_front();
        return dq.front().eval(x);
    }
    static double cross(line a, line b) { return (double)(b.b - a.b) / (double)(a.k - b.k); }
};

//—— 单调队列优化 DP ——
//   dp[0] = base0；对 i>=1：dp[i] = g[i] + min_{j∈[i-R,i-L]} (dp[j]+w[j])。
//   返回 dp（长度 n）。若窗口内无合法 j，则 dp[i] 为 inf。
//   要求 1 <= L <= R < n。
//   inf：无合法转移时的哨兵，默认 numeric_limits<T>::max()；
//   对任意精度类型（High_precision_number / High_precision_floating_point_number）
//   请显式传入自己的 INF（其 numeric_limits::max() 只是编译占位返回 0）。
template<typename T>
std::vector<T> sliding_window_min_dp(int n, int L, int R, T base0,
                                     const std::vector<T>& g,
                                     const std::vector<T>& w,
                                     T inf = std::numeric_limits<T>::max()) {
    std::vector<T> dp(n, inf);
    std::deque<int> dq;
    dp[0] = base0;
    auto val = [&](int j) { return dp[j] + w[j]; };
    for (int i = 1; i < n; i++) {
        int addj = i - L;                       // 当前窗口右端候选
        // 只把「非 INF」的 dp 作为候选：INF 说明该位置无合法转移，
        // 若让其入队，w[j]<0 时 inf+w[j]<inf 会污染 dp[i]（真实语义应为保持 INF）
        if (addj >= 0 && dp[addj] != inf) {
            while (!dq.empty() && val(dq.back()) >= val(addj)) dq.pop_back();
            dq.push_back(addj);
        }
        while (!dq.empty() && dq.front() < i - R) dq.pop_front();   // 过期
        if (!dq.empty()) dp[i] = g[i] + val(dq.front());
    }
    return dp;
}

} // namespace better_std
