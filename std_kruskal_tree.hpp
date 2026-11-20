#ifndef BETTER_STD_STD_KRUSKAL_TREE_HPP
#define BETTER_STD_STD_KRUSKAL_TREE_HPP

//===== Kruskal 重构树 =====
//  由无向带权图构建：最小化「路径最大边」的生成树结构。
//  性质：重构树是二叉树，叶子=原图点，内部点=一次合并及其边权；
//        从叶 u 到叶 v 的 LCA 点权 = u,v 间所有路径中「最大边的最小值」（瓶颈值）。
//  API：
//    kruskal_tree(n, edges)  edges 为 {u,v,w}（0-based 点）
//    .node_weight(i) 节点权（叶子为 0）
//    .bottleneck(u,v) 两点间瓶颈值（最小化的最大边）
//    .lca(a,b) / .dist 不适用（这是合并树，非原树距离）
//    .component(v, limit) 只保留权<=limit 的边后 v 所在连通块（叶子集合）
//    .kth_component(...) 见下
#include<vector>
#include<algorithm>
#include<queue>
#include<stack>
#include<utility>
namespace better_std{

struct kruskal_tree{
    struct edge{ int u, v; long long w; };
    int n = 0;                          // 原图点数
    int total = 0;                      // 重构树节点数（<=2n-1）
    std::vector<long long> weight;      // 节点权（叶子 0）
    std::vector<std::vector<int>> ch;   // 孩子（0/2 个）
    std::vector<int> up0;               // 父
    std::vector<int> dfn_, out_;        // DFS 序 / 子树出时间
    std::vector<int> dep_;
    std::vector<std::vector<int>> up_;  // 倍增表
    int timer_ = 0;

    kruskal_tree() {}
    kruskal_tree(int n_, const std::vector<edge>& edges) { build(n_, edges); }

    void build(int n_, const std::vector<edge>& es) {
        n = n_;
        total = n;
        weight.assign(2*n > 0 ? 2*n : 1, 0);
        ch.assign(2*n > 0 ? 2*n : 1, {});
        up0.assign(2*n > 0 ? 2*n : 1, -1);
        std::vector<int> fa(2*n > 0 ? 2*n : 1);
        for (int i = 0; i < 2*n; i++) fa[i] = i;
        std::vector<edge> sorted(es);
        std::sort(sorted.begin(), sorted.end(),
                  [](const edge& a, const edge& b){ return a.w < b.w; });
        auto find = [&](int x){ while (fa[x] != x) { fa[x] = fa[fa[x]]; x = fa[x]; } return x; };
        for (const edge& e : sorted) {
            int a = find(e.u), b = find(e.v);
            if (a == b) continue;
            int id = total++;
            weight[id] = e.w;
            ch[id].push_back(a);
            ch[id].push_back(b);
            up0[a] = id; up0[b] = id;
            fa[a] = id; fa[b] = id;
            fa[id] = id;
        }
        weight.resize(total); ch.resize(total); up0.resize(total);
        // 找根
        int root = total - 1;
        for (int i = 0; i < total; i++) if (up0[i] == -1) { root = i; break; }
        // DFS 序（显式栈，避免深递归栈溢出）
        dfn_.assign(total, 0); out_.assign(total, 0); dep_.assign(total, 0);
        std::vector<std::pair<int,int>> stk;
        stk.push_back({root, 0});
        while (!stk.empty()) {
            int u = stk.back().first, idx = stk.back().second;
            if (idx == 0) dfn_[u] = timer_++;
            if (idx < (int)ch[u].size()) {
                int v = ch[u][idx];
                stk.back().second++;
                dep_[v] = dep_[u] + 1;
                stk.push_back({v, 0});
            } else {
                out_[u] = timer_;
                stk.pop_back();
            }
        }
        // 倍增表（以父亲链为准）
        int LOG = 1;
        while ((1 << LOG) <= total) LOG++;
        up_.assign(LOG, std::vector<int>(total, -1));
        for (int i = 0; i < total; i++) up_[0][i] = up0[i];
        for (int k = 1; k < LOG; k++)
            for (int i = 0; i < total; i++) {
                int p = up_[k-1][i];
                up_[k][i] = p < 0 ? -1 : up_[k-1][p];
            }
    }

    bool is_ancestor(int a, int b) const { return dfn_[a] <= dfn_[b] && out_[b] <= out_[a]; }

    int lca(int a, int b) const {
        if (is_ancestor(a, b)) return a;
        if (is_ancestor(b, a)) return b;
        int LOG = (int)up_.size();
        for (int k = LOG-1; k >= 0; k--) {
            int p = up_[k][a];
            if (p >= 0 && !is_ancestor(p, b)) a = p;
        }
        return up0[a];
    }

    long long bottleneck(int u, int v) const { return weight[lca(u, v)]; }

    // 只保留权 <= limit 的边后，原图点 v 所在连通块的叶子下标集合
    std::vector<int> component(int v, long long limit) const {
        int u = v;
        int LOG = (int)up_.size();
        for (int k = LOG-1; k >= 0; k--) {
            int p = up_[k][u];
            if (p >= 0 && weight[p] <= limit) u = p;
        }
        std::vector<int> res;
        std::stack<int> st; st.push(u);
        while (!st.empty()) {
            int x = st.top(); st.pop();
            if (x < n && dfn_[x] >= 0) res.push_back(x);
            for (int c : ch[x]) st.push(c);
        }
        std::sort(res.begin(), res.end());
        return res;
    }
};

} // namespace better_std
#endif // BETTER_STD_STD_KRUSKAL_TREE_HPP
