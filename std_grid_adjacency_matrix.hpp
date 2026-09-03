#pragma once
#include<iostream>
#include<vector>
#include<utility>
#include"std_grid.hpp"
using namespace better_std;
namespace better_std{
    //row×col 网格图的邻接矩阵（默认 4 邻接，构造参数 diag=true 时含 8 邻接）
    //矩阵元素为 int：0 表示无边，1 表示有边（也可 set_edge 写入权值）
    template<int row,int col>
    class Grid_adjacency_matrix : public std_grid<row,col>{
        private:
            std::vector<std::vector<int>> mat;//mat[u][v]
            void add_edge(int u,int v) override{ mat[u][v]=1; }
        public:
            Grid_adjacency_matrix(bool diag=false){
                this->node_count=row*col;
                mat.assign(this->node_count,std::vector<int>(this->node_count,0));
                this->build(diag);
            }
            int edge(int u,int v) const{ return mat[u][v]; }
            int operator()(int u,int v) const{ return mat[u][v]; }
            //邻居列表（为与图算法包的 g[u] 接口统一；每次 O(n) 扫描）
            std::vector<int> operator[](int u) const{
                std::vector<int> res;
                for(int v=0;v<this->node_count;v++) if(mat[u][v]) res.push_back(v);
                return res;
            }
            void set_edge(int u,int v,int w=1){ mat[u][v]=w; }
            int degree(int node) const override{
                int d=0;
                for(int v=0;v<this->node_count;v++) if(mat[node][v]) d++;
                return d;
            }
            void print() const override{
                for(int u=0;u<this->node_count;u++){
                    for(int v=0;v<this->node_count;v++) std::cout<<mat[u][v]<<" ";
                    std::cout<<"\n";
                }
            }
    };
}
