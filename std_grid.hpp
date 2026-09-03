#pragma once
#include<iostream>
#include<vector>
#include<utility>
namespace better_std{
    //row×col 图接口基类：维护节点编号与坐标映射
    //派生类只需实现 add_edge（记录一条边）、degree、print 即可。
    //网格图（Grid_adjacency_list / Grid_adjacency_matrix）用 build(diag) 连 4/8 邻接边；
    //树（std_rootless_tree / std_tree / std_binary_tree）不要调用 build()，
    //改用公开的 add_edge 手动建边（保证无环、连通即为一棵树）。
    template<int row,int col>
    class std_grid{
        protected:
            int node_count;
            int id(int r,int c) const{ return r*col+c; }
        public:
            std_grid():node_count(row*col){}
            virtual void add_edge(int u,int v)=0;//public：图与树统一公开的加边接口
            int get_row() const{ return row; }
            int get_col() const{ return col; }
            int size() const{ return node_count; }
            int node_id(int r,int c) const{ return id(r,c); }
            std::pair<int,int> coord(int node) const{ return {node/col,node%col}; }
            //默认 4 邻接；diag=true 时含 8 邻接。网格专用：把网格相邻节点全部连边
            void build(bool diag=false){
                for(int r=0;r<row;r++){
                    for(int c=0;c<col;c++){
                        int u=id(r,c);
                        if(r>0)        add_edge(u,id(r-1,c));
                        if(r+1<row)    add_edge(u,id(r+1,c));
                        if(c>0)        add_edge(u,id(r,c-1));
                        if(c+1<col)    add_edge(u,id(r,c+1));
                        if(diag){
                            if(r>0&&c>0)        add_edge(u,id(r-1,c-1));
                            if(r>0&&c+1<col)    add_edge(u,id(r-1,c+1));
                            if(r+1<row&&c>0)    add_edge(u,id(r+1,c-1));
                            if(r+1<row&&c+1<col)add_edge(u,id(r+1,c+1));
                        }
                    }
                }
            }
            virtual int degree(int node) const=0;
            virtual void print() const=0;
            virtual ~std_grid(){}
    };
}
