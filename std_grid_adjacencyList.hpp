#pragma once
#include<iostream>
#include<vector>
#include<utility>
#include"std_grid.hpp"
using namespace better_std;
namespace better_std{
    //row×col 网格图的邻接表（默认 4 邻接，构造参数 diag=true 时含 8 邻接）
    template<int row,int col>
    class Grid_adjacency_list : public std_grid<row,col>{
        private:
            std::vector<std::vector<int>> adj;//adj[node] = 邻居节点编号列表
            void add_edge(int u,int v) override{ adj[u].push_back(v); }
        public:
            Grid_adjacency_list(bool diag=false){
                this->node_count=row*col;
                adj.assign(this->node_count,std::vector<int>());
                this->build(diag);
            }
            const std::vector<int>& operator[](int node) const{ return adj[node]; }
            std::vector<int> neighbors(int r,int c) const{ return adj[this->id(r,c)]; }
            int operator()(int u,int v) const{ return edge(u,v)?1:0; }//默认边权 1
            int degree(int node) const override{ return (int)adj[node].size(); }
            bool edge(int u,int v) const{
                for(int x:adj[u]) if(x==v) return true;
                return false;
            }
            void print() const override{
                for(int u=0;u<this->node_count;u++){
                    std::cout<<"node "<<u<<" ("<<(u/col)<<","<<(u%col)<<") :";
                    for(int v:adj[u]) std::cout<<" "<<v;
                    std::cout<<"\n";
                }
            }
    };
}
