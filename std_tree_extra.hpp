#ifndef BETTER_STD_STD_TREE_EXTRA_HPP
#define BETTER_STD_STD_TREE_EXTRA_HPP

//===== better_std 树扩展：轻量 LCA + 树直径/重心 + 虚树 =====
//   自包含（邻接表 0..n-1），不耦合网格类模板。
//   · tree_lca：倍增 LCA，支持 lca/depth/dist/kth_ancestor。
//   · tree_diameter(n,edges)：返回 {直径长度, 端点u, 端点v}（无权）。
//   · tree_centroid(n,edges)：返回一个重心。
//   · virtual_tree(n,edges,key_nodes,lca)：构建虚树（返回虚树邻接表 + 虚树节点对应的原节点编号）。
//     虚树边权为原树两点距离（无权树即深度差）。
#include<vector>
#include<queue>
#include<algorithm>
#include<utility>
namespace better_std{

//—— 轻量倍增 LCA（无根树，0..n-1）——
class tree_lca{
    int n_, LOG_;
    std::vector<std::vector<int>> up_;   // up_[k][u]
    std::vector<int> dep_, dfn_;
public:
    tree_lca() {}
    tree_lca(int n,const std::vector<std::vector<int>>& g){ build(n,g); }
    void build(int n,const std::vector<std::vector<int>>& g){
        n_=n; LOG_=1;
        while((1<<LOG_)<=n) LOG_++;
        up_.assign(LOG_,std::vector<int>(n,-1));
        dep_.assign(n,0); dfn_.assign(n,-1);
        std::vector<int> par(n,-2), order;
        order.reserve(n);
        par[0]=-1; order.push_back(0);
        // 先 BFS 定父子与深度，再 DFS 序编号（虚树需要 DFS 序，BFS 序不行）
        for(size_t i=0;i<order.size();i++){
            int u=order[i];
            for(int v:g[u]) if(v!=par[u]){
                par[v]=u; dep_[v]=dep_[u]+1; order.push_back(v);
            }
        }
        // 显式栈 DFS 求 dfn
        int timer=0;
        std::vector<std::pair<int,size_t>> st;
        st.push_back({0,0});
        while(!st.empty()){
            int u=st.back().first;
            size_t& idx=st.back().second;
            if(idx==0) dfn_[u]=timer++;
            if(idx<g[u].size()){
                int v=g[u][idx++];
                if(v!=par[u]) st.push_back({v,0});
            } else st.pop_back();
        }
        for(int u=0;u<n;u++) up_[0][u]=par[u];
        for(int k=1;k<LOG_;k++)
            for(int u=0;u<n;u++)
                up_[k][u]=(up_[k-1][u]<0?-1:up_[k-1][up_[k-1][u]]);
    }
    int depth(int u) const { return dep_[u]; }
    int dfn(int u) const { return dfn_[u]; }
    int lca(int a,int b) const {
        if(dep_[a]<dep_[b]) std::swap(a,b);
        int diff=dep_[a]-dep_[b];
        for(int k=0;k<LOG_;k++) if(diff&(1<<k)) a=up_[k][a];
        if(a==b) return a;
        for(int k=LOG_-1;k>=0;k--) if(up_[k][a]!=up_[k][b]){ a=up_[k][a]; b=up_[k][b]; }
        return up_[0][a];
    }
    int dist(int a,int b) const { int c=lca(a,b); return dep_[a]+dep_[b]-2*dep_[c]; }
    int kth_ancestor(int u,int k) const {   // 第 k 级祖先（k=0 为自身）
        for(int i=0;i<LOG_;i++) if(k&(1<<i)) u=up_[i][u];
        return u;
    }
};

//—— 树直径：返回直径边数，u/v 输出两端点 ——
inline int tree_diameter(const std::vector<std::vector<int>>& g,int& u,int& v){
    int n=(int)g.size();
    if(n==0){ u=v=0; return 0; }
    auto bfs=[&](int s,std::vector<int>& d){
        d.assign(n,-1);
        std::queue<int> q; q.push(s); d[s]=0;
        int far=s;
        while(!q.empty()){
            int x=q.front(); q.pop();
            if(d[x]>d[far]) far=x;
            for(int y:g[x]) if(d[y]<0){ d[y]=d[x]+1; q.push(y); }
        }
        return far;
    };
    std::vector<int> d;
    u=bfs(0,d);
    v=bfs(u,d);
    return d[v];
}

//—— 树重心：返回一个使最大子树最小的点 ——
inline int tree_centroid(const std::vector<std::vector<int>>& g){
    int n=(int)g.size();
    if(n==0) return -1;
    std::vector<int> sz(n,1), parent(n,-2);
    parent[0]=-1;
    std::vector<int> order; order.reserve(n); order.push_back(0);
    for(size_t i=0;i<order.size();i++){
        int u=order[i];
        for(int v:g[u]) if(v!=parent[u]){ parent[v]=u; order.push_back(v); }
    }
    for(int i=(int)order.size()-1;i>=0;i--){
        int u=order[i]; sz[u]=1;
        for(int v:g[u]) if(parent[v]==u) sz[u]+=sz[v];
    }
    int best=0, bestMax=n+1;
    for(int u=0;u<n;u++){
        int mx=n-sz[u];
        for(int v:g[u]) if(parent[v]==u) mx=std::max(mx,sz[v]);
        if(mx<bestMax){ bestMax=mx; best=u; }
    }
    return best;
}

//—— 虚树构建 ——
// 输入：原树邻接表 g、LCA 结构、关键点 key（任意顺序，可重复）。
// 返回：pair<虚树邻接表(带权), 虚树节点对应的原树编号>。虚树根为 key 中 LCA 最深公共祖先。
inline std::pair<std::vector<std::vector<std::pair<int,int>>>,std::vector<int>>
virtual_tree(const std::vector<std::vector<int>>& g,const tree_lca& lca,std::vector<int> key){ (void)g;
    // 按 dfs 序排序关键点并去重
    std::sort(key.begin(),key.end(),[&](int a,int b){ return lca.dfn(a)<lca.dfn(b); });
    key.erase(std::unique(key.begin(),key.end()),key.end());
    if(key.empty()) return {{},{}};
    // 收集相邻两点的 LCA（标准虚树构造）
    std::vector<int> nodes=key;
    for(size_t i=0;i+1<key.size();i++) nodes.push_back(lca.lca(key[i],key[i+1]));
    std::sort(nodes.begin(),nodes.end(),[&](int a,int b){ return lca.dfn(a)<lca.dfn(b); });
    nodes.erase(std::unique(nodes.begin(),nodes.end()),nodes.end());
    int m=(int)nodes.size();
    // nodes 已按 dfn 排序，下标即虚树节点索引
    auto cmpdfn=[&](int a,int b){ return lca.dfn(a)<lca.dfn(b); };
    auto id=[&](int u){ return (int)(std::lower_bound(nodes.begin(),nodes.end(),u,cmpdfn)-nodes.begin()); };
    std::vector<std::vector<std::pair<int,int>>> vt(m);
    auto link=[&](int a,int b){ int w=lca.dist(a,b); vt[id(a)].push_back({id(b),w}); vt[id(b)].push_back({id(a),w}); };
    std::vector<int> stk;
    for(int u:nodes){
        while(!stk.empty() && lca.lca(stk.back(),u)!=stk.back()) stk.pop_back();
        if(!stk.empty()) link(stk.back(),u);
        stk.push_back(u);
    }
    return {vt,nodes};
}

} // namespace better_std
#endif // BETTER_STD_STD_TREE_EXTRA_HPP
