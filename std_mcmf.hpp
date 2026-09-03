#pragma once
#include<vector>
#include<deque>
#include<algorithm>
#include<limits>
namespace better_std{
    //最小费用最大流（SPFA 增广最短路；支持负费用边与反向边）
    //add_edge(u,v,cap,cost)；min_cost_max_flow(s,t) 返回 {最大流, 最小费用}
    template<typename T=long long>
    class std_mcmf{
        private:
            struct Edge{ int to,rev; T cap,cost; };
            int n_;
            std::vector<std::vector<Edge>> g_;
        public:
            std_mcmf(int n=0):n_(n),g_(n){}
            void add_edge(int u,int v,T cap,T cost){
                if(cap<=T()) return;
                g_[u].push_back({v,(int)g_[v].size(),cap,cost});
                g_[v].push_back({u,(int)g_[u].size()-1,T(),-cost});
            }
            std::pair<T,T> min_cost_max_flow(int s,int t){
                const T INF=std::numeric_limits<T>::max()/4;
                T flow=T(),cost=T();
                while(true){
                    std::vector<T> dist(n_,INF);
                    std::vector<int> pv(n_,-1),pe(n_,-1);
                    std::vector<char> inq(n_,false);
                    std::deque<int> q;
                    dist[s]=T();q.push_back(s);inq[s]=true;
                    while(!q.empty()){
                        int u=q.front();q.pop_front();inq[u]=false;
                        for(int i=0;i<(int)g_[u].size();i++){
                            Edge& e=g_[u][i];
                            if(e.cap>T() && dist[u]+e.cost<dist[e.to]){
                                dist[e.to]=dist[u]+e.cost;
                                pv[e.to]=u;pe[e.to]=i;
                                if(!inq[e.to]){ q.push_back(e.to); inq[e.to]=true; }
                            }
                        }
                    }
                    if(pv[t]==-1) break;//无增广路
                    T f=INF;
                    for(int v=t;v!=s;v=pv[v]) f=std::min(f,g_[pv[v]][pe[v]].cap);
                    for(int v=t;v!=s;v=pv[v]){
                        Edge& e=g_[pv[v]][pe[v]];
                        e.cap-=f;
                        g_[v][e.rev].cap+=f;
                    }
                    flow+=f;
                    cost+=f*dist[t];
                }
                return {flow,cost};
            }
    };
}
