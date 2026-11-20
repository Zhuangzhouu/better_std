#ifndef BETTER_STD_STD_BLOCK_CUT_TREE_HPP
#define BETTER_STD_STD_BLOCK_CUT_TREE_HPP

//===== better_std 圆方树（Block-Cut Tree）/ 仙人掌 =====
//   无向图的点双连通分量圆方树：原点 0..n-1，方点 n..n+cnt-1。
//   build(g)：Tarjan 栈构造；返回新图（圆方树邻接表）与总节点数。
//   cactus_dist(u,v)：仙人掌（每条边属于至多一个环）上两点最短路；先 build 圆方树再 BFS LCA。
//   简化：本实现提供圆方树构造 + 圆方树上两点路径查询（边权均为 1，方点-原点距离=1）。
#include<vector>
#include<stack>
#include<queue>
#include<functional>
#include<algorithm>
#include<utility>
namespace better_std{

struct block_cut_tree{
    std::vector<std::vector<int>> tree;   // 圆方树邻接表
    int total=0;                          // 总节点数（含方点）
    int n_orig=0;

    block_cut_tree() {}
    explicit block_cut_tree(const std::vector<std::vector<int>>& g){ build(g); }

    void build(const std::vector<std::vector<int>>& g){
        int n=(int)g.size();
        n_orig=n;
        std::vector<int> dfn(n,0), low(n,0);
        std::vector<int> stk;
        int ts=0;
        total=n;
        tree.assign(n,{});   // 先放原点，方点按需 push
        // 迭代版 Tarjan 太复杂，用递归 std::function（C++11 兼容）
        std::function<void(int,int)> dfs=[&](int u,int pe){
            dfn[u]=low[u]=++ts;
            stk.push_back(u);
            for(int v:g[u]){
                if(v==pe) continue;
                if(!dfn[v]){
                    dfs(v,u);
                    low[u]=std::min(low[u],low[v]);
                    if(low[v]>=dfn[u]){
                        // 新建方点
                        int sq=total++;
                        tree.push_back({});
                        while(true){
                            int x=stk.back(); stk.pop_back();
                            tree[sq].push_back(x);
                            tree[x].push_back(sq);
                            if(x==v) break;
                        }
                        tree[sq].push_back(u);
                        tree[u].push_back(sq);
                    }
                } else low[u]=std::min(low[u],dfn[v]);
            }
        };
        for(int i=0;i<n;i++) if(!dfn[i]){ stk.clear(); dfs(i,-1); }
    }
    int size() const { return total; }
    // 圆方树上两点距离（BFS，无权）
    int dist(int a,int b) const {
        if(a<0||b<0||a>=total||b>=total) return -1;
        std::vector<int> d(total,-1);
        std::queue<int> q; q.push(a); d[a]=0;
        while(!q.empty()){
            int u=q.front(); q.pop();
            if(u==b) return d[u];
            for(int v:tree[u]) if(d[v]<0){ d[v]=d[u]+1; q.push(v); }
        }
        return -1;
    }
};

// 仙人掌最短路：利用圆方树 —— 方点内用一个小 BFS 处理环上两段取最小。
// 简化通用做法：直接在原图 BFS（对大多数竞赛规模够用，但非 O(log n)）。
inline int cactus_dist(const std::vector<std::vector<int>>& g,int u,int v){
    int n=(int)g.size();
    if(u<0||v<0||u>=n||v>=n) return -1;
    std::vector<int> d(n,-1);
    std::queue<int> q; q.push(u); d[u]=0;
    while(!q.empty()){
        int x=q.front(); q.pop();
        if(x==v) return d[x];
        for(int y:g[x]) if(d[y]<0){ d[y]=d[x]+1; q.push(y); }
    }
    return -1;
}

} // namespace better_std
#endif // BETTER_STD_STD_BLOCK_CUT_TREE_HPP
