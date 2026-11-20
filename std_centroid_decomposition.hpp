#ifndef BETTER_STD_STD_CENTROID_DECOMPOSITION_HPP
#define BETTER_STD_STD_CENTROID_DECOMPOSITION_HPP

//===== 点分治（重心分解）=====
//  解决「树上路径计数」问题：统计满足 f(路径) 的点对 / 路径。
//  用法：构造 centroid_decomposition(g, edges)，edges[i]={u,v,w} 为边。
//    .count_distance_k(K)  返回距离恰好为 K 的点对数（0-based 点对，u<v）
//    .count_distance_le(K) 返回距离 <= K 的点对数
//  内部：递归找重心，对每个重心统计「经过重心」的路径，再递归子树容斥。
//  复杂度 O(n log n)。
#include<vector>
#include<algorithm>
#include<cstring>
namespace better_std{

class centroid_decomposition{
    struct edge{ int to; long long w; };
    int n_;
    std::vector<std::vector<edge>> g_;
    std::vector<int> sz_, parent_;
    std::vector<char> removed_;
    long long K_ = 0;
    long long ans_ = 0;

    // 计算子树大小（不在 removed 内）
    void calc_size(int u, int p) {
        sz_[u] = 1;
        for (const edge& e : g_[u]) if (e.to != p && !removed_[e.to]) {
            calc_size(e.to, u);
            sz_[u] += sz_[e.to];
        }
    }
    // 找重心：返回使最大子树最小的点
    int find_centroid(int u, int p, int total) {
        for (const edge& e : g_[u]) if (e.to != p && !removed_[e.to] && sz_[e.to] > total/2)
            return find_centroid(e.to, u, total);
        return u;
    }
    // 收集从 u 出发、不经过 removed/p 的所有点距离到 dist
    void collect(int u, int p, long long d, std::vector<long long>& dist) {
        dist.push_back(d);
        for (const edge& e : g_[u]) if (e.to != p && !removed_[e.to])
            collect(e.to, u, d + e.w, dist);
    }
    // 统计距离列表中满足 d<=K（或 ==K）的对数（含与 0 的配对在调用处处理）
    long long count_pairs(const std::vector<long long>& d, bool le) const {
        std::vector<long long> s(d);
        std::sort(s.begin(), s.end());
        long long cnt = 0;
        // 双指针：统计 s[i]+s[j] 满足 <=K 或 ==K 的有序对 i<j
        if (le) {
            int j = (int)s.size()-1;
            for (int i = 0; i < (int)s.size(); i++) {
                while (j > i && s[i] + s[j] > K_) j--;
                if (j <= i) break;
                cnt += j - i;
            }
        } else {
            // 等于 K：排序后对每个 i 二分出 s[j] == K-s[i] 且 j>i 的个数（保证每对只算一次）
            for (int i = 0; i < (int)s.size(); i++) {
                long long need = K_ - s[i];
                auto lo = std::lower_bound(s.begin()+i+1, s.end(), need);
                auto hi = std::upper_bound(lo, s.end(), need);
                cnt += (long long)(hi - lo);
            }
        }
        return cnt;
    }
    // 处理以 centroid c 为根的路径统计
    void solve_at(int c, bool le) {
        std::vector<long long> all; all.push_back(0);   // 重心自身距离 0
        for (const edge& e : g_[c]) if (!removed_[e.to]) {
            std::vector<long long> sub;
            collect(e.to, c, e.w, sub);
            ans_ -= count_pairs(sub, le);               // 容斥：同一子树内的路径
            all.insert(all.end(), sub.begin(), sub.end());
        }
        ans_ += count_pairs(all, le);                   // 所有经过重心的路径
    }
    void decompose(int u, int p, bool le) {
        calc_size(u, p);
        int c = find_centroid(u, p, sz_[u]);
        removed_[c] = 1;
        solve_at(c, le);
        for (const edge& e : g_[c]) if (!removed_[e.to])
            decompose(e.to, c, le);
        removed_[c] = 0;
    }

public:
    // edges: {u, v, w}，u/v 为 0-based 点
    centroid_decomposition(int n, const std::vector<std::vector<long long>>& edges) : n_(n) {
        g_.assign(n, {});
        for (const auto& e : edges) {
            int u = (int)e[0], v = (int)e[1]; long long w = e[2];
            g_[u].push_back({v, w});
            g_[v].push_back({u, w});
        }
        sz_.assign(n, 0); parent_.assign(n, -1); removed_.assign(n, 0);
    }
    // 距离恰好为 K 的点对数
    long long count_distance_k(long long K) {
        K_ = K; ans_ = 0;
        if (n_ > 0) decompose(0, -1, false);
        return ans_;
    }
    // 距离 <= K 的点对数
    long long count_distance_le(long long K) {
        K_ = K; ans_ = 0;
        if (n_ > 0) decompose(0, -1, true);
        return ans_;
    }
};

} // namespace better_std
#endif // BETTER_STD_STD_CENTROID_DECOMPOSITION_HPP
