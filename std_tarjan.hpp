#pragma once
//===== better_std 图连通性（Tarjan）：割点 / 桥 / 点双 / 边双 =====
// 输入无向图 g：g[u] 为 u 的邻居列表（节点 0..n-1）
#include<vector>
#include<algorithm>
namespace better_std{

struct tarjan_result{
    std::vector<char> is_cut;                  // 割点标志（0..n-1）
    std::vector<std::pair<int,int>> bridges;  // 桥（无序边，存为 {min,max}）
    std::vector<std::vector<int>> vbcc;       // 边双连通分量（点集）
    std::vector<std::vector<int>> pbcc;       // 点双连通分量（点集）
};

inline tarjan_result tarjan(const std::vector<std::vector<int>>& g){
    int n = (int)g.size();
    tarjan_result res;
    res.is_cut.assign(n, 0);
    std::vector<int> dfn(n + 1, 0), low(n + 1, 0);
    std::vector<int> stk;
    int ts = 0;

    auto dfs = [&](auto& self, int u, int pe) -> void {
        dfn[u] = low[u] = ++ts;
        stk.push_back(u);
        int children = 0;
        for(int v : g[u]){
            if(v == pe) continue;
            if(!dfn[v]){
                children++;
                self(self, v, u);
                low[u] = std::min(low[u], low[v]);
                if(pe != -1 && low[v] >= dfn[u]) res.is_cut[u] = 1;
                if(low[v] >= dfn[u]){               // 点双：弹出 v 子树并加入 u
                    std::vector<int> comp;
                    while(true){
                        int x = stk.back(); stk.pop_back();
                        comp.push_back(x);
                        if(x == v) break;
                    }
                    comp.push_back(u);
                    res.pbcc.push_back(comp);
                }
            } else if(dfn[v] < dfn[u]){             // 回边
                low[u] = std::min(low[u], dfn[v]);
            }
        }
        if(pe == -1 && children >= 2) res.is_cut[u] = 1;
    };

    for(int i = 0; i < n; i++)
        if(!dfn[i]) dfs(dfs, i, -1);

    // 桥：在 DFS 中已记录 dfn/low，树边 (u→v) 满足 low[v] > dfn[u] 即为桥
    for(int u = 0; u < n; u++){
        for(int v : g[u]){
            if(v > u && dfn[v] > dfn[u] && low[v] > dfn[u])
                res.bridges.push_back({u, v});
        }
    }

    // 边双：去掉桥后做连通块
    std::vector<std::vector<int>> ng(n);
    for(int u = 0; u < n; u++)
        for(int v : g[u]){
            int a = std::min(u, v), b = std::max(u, v);
            bool is_bridge = false;
            for(auto& br : res.bridges) if(br == std::pair<int,int>(a, b)){ is_bridge = true; break; }
            if(!is_bridge) ng[u].push_back(v);
        }
    std::vector<char> vis(n, 0);
    for(int i = 0; i < n; i++){
        if(vis[i]) continue;
        std::vector<int> comp;
        std::vector<int> q = {i}; vis[i] = 1;
        while(!q.empty()){
            int u = q.back(); q.pop_back();
            comp.push_back(u);
            for(int v : ng[u]) if(!vis[v]){ vis[v] = 1; q.push_back(v); }
        }
        res.vbcc.push_back(comp);
    }
    return res;
}

} // namespace better_std
