#pragma once
#include<vector>
#include<queue>
#include<deque>
#include<set>
#include<tuple>
#include<algorithm>
#include<functional>
#include<climits>
#include"std_dsu.hpp"
namespace better_std{
    //===== 图算法函数包 =====
    //对任何满足以下接口的图/树类可用（std_grid 系派生类均已实现）：
    //   int size()                      —— 节点数
    //   g[u]（返回邻居编号列表）        —— 遍历邻居
    //   int g(u,v)                      —— 边权（无向图矩阵返回权值，邻接表/树返回 1/0）
    //均为无向/有向通用；dijkstra/kruskal 要求边权非负，spfa 支持负权

    //无权图 BFS：src 到各点的最短边数（不可达 -1）
    template<class G>
    std::vector<int> bfs(const G& g,int src){
        int n=g.size();
        std::vector<int> dist(n,-1),q;
        dist[src]=0;q.push_back(src);
        for(size_t h=0;h<q.size();h++){
            int u=q[h];
            for(int v:g[u]) if(dist[v]<0){ dist[v]=dist[u]+1; q.push_back(v); }
        }
        return dist;
    }
    //DFS 前序访问序列（从 src 出发）
    template<class G>
    std::vector<int> dfs_order(const G& g,int src){
        int n=g.size();
        std::vector<int> res,vis(n,0),stk;
        stk.push_back(src);vis[src]=1;
        while(!stk.empty()){
            int u=stk.back();stk.pop_back();
            res.push_back(u);
            for(int v:g[u]) if(!vis[v]){ vis[v]=1; stk.push_back(v); }
        }
        return res;
    }
    //Dijkstra 单源最短路（边权 = g(u,v)，非负；不可达返回 INF）
    template<class G>
    std::vector<int> dijkstra(const G& g,int src){
        const int INF=INT_MAX/4;
        int n=g.size();
        std::vector<int> dist(n,INF);
        using P=std::pair<int,int>;
        std::priority_queue<P,std::vector<P>,std::greater<P>> pq;
        dist[src]=0;pq.push({0,src});
        while(!pq.empty()){
            P top=pq.top();pq.pop();
            int d=top.first,u=top.second;
            if(d!=dist[u]) continue;
            for(int v:g[u]){
                int w=g(u,v);
                if(w>0 && d+w<dist[v]){ dist[v]=d+w; pq.push({dist[v],v}); }
            }
        }
        return dist;
    }
    //Floyd 全源最短路（返回 n×n；不可达 INF）
    template<class G>
    std::vector<std::vector<int>> floyd(const G& g){
        const int INF=INT_MAX/4;
        int n=g.size();
        std::vector<std::vector<int>> d(n,std::vector<int>(n,INF));
        for(int i=0;i<n;i++) d[i][i]=0;
        for(int u=0;u<n;u++) for(int v:g[u]) d[u][v]=std::min(d[u][v],g(u,v));
        for(int k=0;k<n;k++)
            for(int i=0;i<n;i++)
                for(int j=0;j<n;j++)
                    if(d[i][k]+d[k][j]<d[i][j]) d[i][j]=d[i][k]+d[k][j];
        return d;
    }
    //Kruskal 最小生成树：返回 {总权, 边集}；边权 = g(u,v)，非负；内部用 std_dsu
    template<class G>
    std::pair<int,std::vector<std::pair<int,int>>> kruskal(const G& g){
        int n=g.size();
        std::vector<std::tuple<int,int,int>> es;
        for(int u=0;u<n;u++) for(int v:g[u]) if(u<v) es.push_back({g(u,v),u,v});
        std::sort(es.begin(),es.end());
        std_dsu dsu(n);
        int cost=0;
        std::vector<std::pair<int,int>> mst;
        for(auto& e:es){
            int w=std::get<0>(e),u=std::get<1>(e),v=std::get<2>(e);
            if(dsu.merge(u,v)){ cost+=w; mst.push_back({u,v}); }
        }
        return {cost,mst};
    }
    //拓扑排序（有向图）：返回一个拓扑序；含环返回空
    template<class G>
    std::vector<int> topo_sort(const G& g){
        int n=g.size();
        std::vector<int> indeg(n,0);
        for(int u=0;u<n;u++) for(int v:g[u]) indeg[v]++;
        std::vector<int> q,res;
        for(int u=0;u<n;u++) if(indeg[u]==0) q.push_back(u);
        for(size_t h=0;h<q.size();h++){
            int u=q[h];res.push_back(u);
            for(int v:g[u]) if(--indeg[v]==0) q.push_back(v);
        }
        if((int)res.size()!=n) return {};//有环
        return res;
    }
    //SPFA 单源最短路（支持负权；检测到负环返回空）
    template<class G>
    std::vector<int> spfa(const G& g,int src){
        const int INF=INT_MAX/4;
        int n=g.size();
        std::vector<int> dist(n,INF),cnt(n,0);
        std::vector<char> inq(n,false);
        std::deque<int> q;
        dist[src]=0;q.push_back(src);inq[src]=true;
        while(!q.empty()){
            int u=q.front();q.pop_front();inq[u]=false;
            for(int v:g[u]){
                int w=g(u,v);
                if(dist[u]+w<dist[v]){
                    dist[v]=dist[u]+w;
                    if(!inq[v]){
                        q.push_back(v);inq[v]=true;
                        if(++cnt[v]>n) return {};//负环
                    }
                }
            }
        }
        return dist;
    }
    //无向图判环（用 std_dsu）：有环返回 true
    template<class G>
    bool has_cycle(const G& g){
        int n=g.size();
        std_dsu dsu(n);
        for(int u=0;u<n;u++)
            for(int v:g[u])
                if(u<v){
                    if(dsu.same(u,v)) return true;
                    dsu.merge(u,v);
                }
        return false;
    }
    //Bellman-Ford 单源最短路（支持负权；检测到负环返回空）
    template<class G>
    std::vector<int> bellman_ford(const G& g,int src){
        const int INF=INT_MAX/4;
        int n=g.size();
        std::vector<int> dist(n,INF);
        dist[src]=0;
        for(int i=0;i<n-1;i++){
            bool upd=false;
            for(int u=0;u<n;u++) if(dist[u]<INF)
                for(int v:g[u]){
                    int w=g(u,v);
                    if(dist[u]+w<dist[v]){ dist[v]=dist[u]+w; upd=true; }
                }
            if(!upd) break;
        }
        for(int u=0;u<n;u++) if(dist[u]<INF)
            for(int v:g[u])
                if(dist[u]+g(u,v)<dist[v]) return {};//负环
        return dist;
    }
    //强连通分量（Kosaraju）：返回每个节点所属分量编号 0..k-1
    template<class G>
    std::vector<int> scc(const G& g){
        int n=g.size();
        std::vector<std::vector<int>> rg(n);//反向图
        for(int u=0;u<n;u++) for(int v:g[u]) rg[v].push_back(u);
        std::vector<int> order,vis(n,0);
        std::function<void(int)> dfs1=[&](int u){
            vis[u]=1;
            for(int v:g[u]) if(!vis[v]) dfs1(v);
            order.push_back(u);
        };
        for(int u=0;u<n;u++) if(!vis[u]) dfs1(u);
        std::vector<int> comp(n,-1);
        int cid=0;
        std::function<void(int)> dfs2=[&](int u){
            comp[u]=cid;
            for(int v:rg[u]) if(comp[v]==-1) dfs2(v);
        };
        for(int i=n-1;i>=0;i--){
            int u=order[i];
            if(comp[u]==-1){ dfs2(u); cid++; }
        }
        return comp;
    }
    //传递闭包（有向图，布尔可达矩阵）
    template<class G>
    std::vector<std::vector<char>> transitive_closure(const G& g){
        int n=g.size();
        std::vector<std::vector<char>> r(n,std::vector<char>(n,0));
        for(int i=0;i<n;i++){
            r[i][i]=1;
            for(int v:g[i]) r[i][v]=1;
        }
        for(int k=0;k<n;k++)
            for(int i=0;i<n;i++) if(r[i][k])
                for(int j=0;j<n;j++) if(r[k][j]) r[i][j]=1;
        return r;
    }
    //二分图判定（BFS 染色）：返回 {是否二分, 颜色数组(0/1)}；非二分时 second 为空
    template<class G>
    std::pair<bool,std::vector<int>> is_bipartite(const G& g){
        int n=g.size();
        std::vector<int> col(n,-1);
        for(int s=0;s<n;s++) if(col[s]==-1){
            col[s]=0;
            std::vector<int> q;q.push_back(s);
            for(size_t h=0;h<q.size();h++){
                int u=q[h];
                for(int v:g[u]){
                    if(col[v]==-1){ col[v]=col[u]^1; q.push_back(v); }
                    else if(col[v]==col[u]) return {false,{}};
                }
            }
        }
        return {true,col};
    }
    //强连通缩点：用 scc 求分量后缩成 DAG（邻接表，去重边）
    template<class G>
    std::vector<std::vector<int>> scc_condense(const G& g){
        std::vector<int> comp=scc(g);
        int c=0;
        for(int x:comp) c=std::max(c,x+1);
        std::vector<std::vector<int>> dag(c);
        for(int u=0;u<(int)g.size();u++)
            for(int v:g[u]){
                int cu=comp[u],cv=comp[v];
                if(cu!=cv) dag[cu].push_back(cv);
            }
        for(auto& e:dag){
            std::sort(e.begin(),e.end());
            e.erase(std::unique(e.begin(),e.end()),e.end());
        }
        return dag;
    }
    //无向图欧拉回路（Hierholzer）：所有点度为偶才存在；返回节点序列（边数+1 个，首尾相接），否则空
    template<class G>
    std::vector<int> euler_circuit(const G& g){
        int n=g.size();
        std::vector<std::multiset<int>> adj(n);
        for(int u=0;u<n;u++) for(int v:g[u]) adj[u].insert(v);
        for(int u=0;u<n;u++) if((int)adj[u].size()%2) return {};
        int start=0;
        while(start<n && adj[start].empty()) start++;
        if(start==n) return {};
        std::vector<int> st,path;
        st.push_back(start);
        while(!st.empty()){
            int u=st.back();
            if(adj[u].empty()){ path.push_back(u); st.pop_back(); }
            else{
                int v=*adj[u].begin();
                adj[u].erase(adj[u].begin());
                adj[v].erase(adj[v].find(u));
                st.push_back(v);
            }
        }
        for(int u=0;u<n;u++) if(!adj[u].empty()) return {};//非连通/边未走完
        std::reverse(path.begin(),path.end());
        return path;
    }
    //无向图欧拉路径：0 或 2 个奇度点才存在；返回路径（含两端），否则空
    template<class G>
    std::vector<int> euler_path(const G& g){
        int n=g.size();
        std::vector<std::multiset<int>> adj(n);
        for(int u=0;u<n;u++) for(int v:g[u]) adj[u].insert(v);
        int odd=0,start=0;
        for(int u=0;u<n;u++) if((int)adj[u].size()%2){ odd++; if(odd==1) start=u; }
        if(odd!=0&&odd!=2) return {};
        while(start<n && adj[start].empty()) start++;
        if(start==n) return {};
        std::vector<int> st,path;
        st.push_back(start);
        while(!st.empty()){
            int u=st.back();
            if(adj[u].empty()){ path.push_back(u); st.pop_back(); }
            else{
                int v=*adj[u].begin();
                adj[u].erase(adj[u].begin());
                adj[v].erase(adj[v].find(u));
                st.push_back(v);
            }
        }
        for(int u=0;u<n;u++) if(!adj[u].empty()) return {};
        std::reverse(path.begin(),path.end());
        return path;
    }
    //二分图最大匹配（匈牙利）：左部 0..n1-1，右部 n1..n1+n2-1（g 为无向邻居）
    //返回匹配数，match 记录每个左部节点匹配的右部节点（-1 未匹配）
    template<class G>
    int hungarian(const G& g,int n1,int n2,std::vector<int>& match){
        std::vector<int> mr(n1,-1);//右部→左部
        std::function<bool(int,std::vector<char>&)> dfs=
            [&](int u,std::vector<char>& vis)->bool{
                for(int v:g[u]){
                    if(v<n1||v>=n1+n2) continue;
                    int w=v-n1;
                    if(vis[w]) continue;
                    vis[w]=1;
                    if(mr[w]==-1 || dfs(mr[w],vis)){ mr[w]=u; return true; }
                }
                return false;
            };
        int res=0;
        for(int u=0;u<n1;u++){
            std::vector<char> vis(n2,0);
            if(dfs(u,vis)) res++;
        }
        match.assign(n1,-1);
        for(int w=0;w<n2;w++) if(mr[w]!=-1) match[mr[w]]=w+n1;
        return res;
    }
    //Prim 最小生成树（稠密图 O(n²)）：返回 {总权, 边集}；边权 = g(u,v)，非负；不连通时少边
    template<class G>
    std::pair<int,std::vector<std::pair<int,int>>> prim(const G& g){
        const int INF=INT_MAX/4;
        int n=g.size();
        std::vector<int> low(n,INF),pre(n,-1);
        std::vector<char> in(n,0);
        low[0]=0;
        int cost=0;
        std::vector<std::pair<int,int>> mst;
        for(int it=0;it<n;it++){
            int u=-1;
            for(int i=0;i<n;i++) if(!in[i] && (u==-1||low[i]<low[u])) u=i;
            if(u==-1||low[u]>=INF) break;
            in[u]=1;
            cost+=low[u];
            if(pre[u]!=-1) mst.push_back({pre[u],u});
            for(int v:g[u]){
                int w=g(u,v);
                if(!in[v] && w>0 && w<low[v]){ low[v]=w; pre[v]=u; }
            }
        }
        return {cost,mst};
    }
}
