#pragma once
//===== better_std::std_bipartite_matching —— 二分图匹配 / 多重匹配 =====
//  hopcroft_karp            : 单位容量二分图最大匹配（O(E√V)），返回匹配大小与 matchL/matchR。
//  bipartite_multi_matching : 左右带容量（多重）匹配，基于紧凑 Dinic 最大流，
//                             返回最大匹配数与每条输入边是否被选中。
#include<bits/stdc++.h>
namespace better_std {

//—— Hopcroft–Karp ——
struct hopcroft_karp {
    int nL, nR;
    std::vector<std::vector<int>> adj;
    std::vector<int> matchL, matchR, dist;
    hopcroft_karp(int nl, int nr) : nL(nl), nR(nr), adj(nl), matchL(nl, -1), matchR(nr, -1) {}
    void add(int u, int v) { adj[u].push_back(v); }
    bool bfs() {
        std::queue<int> q; dist.assign(nL, -1);
        for (int i = 0; i < nL; i++) if (matchL[i] == -1) { dist[i] = 0; q.push(i); }
        bool found = false;
        while (!q.empty()) {
            int u = q.front(); q.pop();
            for (int v : adj[u]) {
                int w = matchR[v];
                if (w == -1) found = true;
                else if (dist[w] < 0) { dist[w] = dist[u] + 1; q.push(w); }
            }
        }
        return found;
    }
    bool dfs(int u) {
        for (int v : adj[u]) {
            int w = matchR[v];
            if (w == -1 || (dist[w] == dist[u] + 1 && dfs(w))) {
                matchL[u] = v; matchR[v] = u; return true;
            }
        }
        dist[u] = -1; return false;
    }
    int max_matching() {
        int ans = 0;
        while (bfs()) for (int i = 0; i < nL; i++) if (matchL[i] == -1 && dfs(i)) ans++;
        return ans;
    }
};

//—— 紧凑 Dinic 最大流 ——
struct dinic {
    struct edge { int to, rev; long long cap; };
    int n;
    std::vector<std::vector<edge>> g;
    std::vector<int> level, it;
    explicit dinic(int n_) : n(n_), g(n_) {}
    int add_edge(int u, int v, long long cap) {
        g[u].push_back({v, (int)g[v].size(), cap});
        int fidx = (int)g[u].size() - 1;
        g[v].push_back({u, fidx, 0});
        return fidx;
    }
    bool bfs(int s, int t) {
        level.assign(n, -1); level[s] = 0;
        std::queue<int> q; q.push(s);
        while (!q.empty()) {
            int u = q.front(); q.pop();
            for (auto& e : g[u]) if (e.cap > 0 && level[e.to] < 0) { level[e.to] = level[u] + 1; q.push(e.to); }
        }
        return level[t] >= 0;
    }
    long long dfs(int u, int t, long long f) {
        if (u == t) return f;
        for (int& i = it[u]; i < (int)g[u].size(); i++) {
            edge& e = g[u][i];
            if (e.cap > 0 && level[e.to] == level[u] + 1) {
                long long d = dfs(e.to, t, std::min(f, e.cap));
                if (d > 0) { e.cap -= d; g[e.to][e.rev].cap += d; return d; }
            }
        }
        return 0;
    }
    long long maxflow(int s, int t) {
        const long long INF = (long long)1e18;
        long long flow = 0;
        while (bfs(s, t)) {
            it.assign(n, 0);
            long long f;
            while ((f = dfs(s, t, INF)) > 0) flow += f;
        }
        return flow;
    }
};

//—— 多重匹配（左右带容量）——
struct bipartite_multi_matching_result {
    long long max_matches;
    std::vector<bool> used;   // 对应输入每条边是否被选中
};

inline bipartite_multi_matching_result
bipartite_multi_matching(int L, int R,
                         const std::vector<int>& capL,
                         const std::vector<int>& capR,
                         const std::vector<std::pair<int, int>>& edges) {
    int S = 0, baseL = 1, baseR = 1 + L, T = 1 + L + R;
    dinic gph(T + 1);
    for (int i = 0; i < L; i++) gph.add_edge(S, baseL + i, capL[i]);
    for (int j = 0; j < R; j++) gph.add_edge(baseR + j, T, capR[j]);
    std::vector<int> fwd; fwd.reserve(edges.size());
    for (auto& e : edges) fwd.push_back(gph.add_edge(baseL + e.first, baseR + e.second, 1));

    long long flow = gph.maxflow(S, T);
    std::vector<bool> used(edges.size(), false);
    for (size_t i = 0; i < edges.size(); i++) {
        int u = baseL + edges[i].first;
        used[i] = (gph.g[u][fwd[i]].cap == 0);   // 原容量 1，被用则残留 0
    }
    return {flow, used};
}

} // namespace better_std
