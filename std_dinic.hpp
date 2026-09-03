#pragma once
#include<vector>
#include<queue>
#include<algorithm>
#include<limits>
namespace better_std{
    //Dinic 最大流（T 为容量类型，默认 long long）
    //add_edge 建带反向边的有向边；max_flow(s,t) 求最大流
    template<typename T=long long>
    class std_dinic{
        private:
            struct Edge{ int to,rev; T cap; };
            int n_;
            std::vector<std::vector<Edge>> g_;
            std::vector<int> level_,it_;
            bool bfs(int s,int t){
                level_.assign(n_,-1);
                std::queue<int> q;
                level_[s]=0;q.push(s);
                while(!q.empty()){
                    int u=q.front();q.pop();
                    for(auto& e:g_[u]) if(e.cap>T() && level_[e.to]<0){
                        level_[e.to]=level_[u]+1;
                        q.push(e.to);
                    }
                }
                return level_[t]>=0;
            }
            T dfs(int u,int t,T f){
                if(u==t) return f;
                for(int& i=it_[u];i<(int)g_[u].size();i++){
                    Edge& e=g_[u][i];
                    if(e.cap>T() && level_[e.to]==level_[u]+1){
                        T d=dfs(e.to,t,std::min(f,e.cap));
                        if(d>T()){
                            e.cap-=d;
                            g_[e.to][e.rev].cap+=d;
                            return d;
                        }
                    }
                }
                return T();
            }
        public:
            std_dinic(int n=0):n_(n),g_(n){}
            void add_edge(int u,int v,T cap){
                if(cap<=T()) return;
                g_[u].push_back({v,(int)g_[v].size(),cap});
                g_[v].push_back({u,(int)g_[u].size()-1,T()});
            }
            T max_flow(int s,int t){
                T flow=T();
                while(bfs(s,t)){
                    it_.assign(n_,0);
                    while(true){
                        T f=dfs(s,t,std::numeric_limits<T>::max());
                        if(!(f>T())) break;
                        flow+=f;
                    }
                }
                return flow;
            }
    };
}
