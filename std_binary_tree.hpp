#pragma once
#include<iostream>
#include<vector>
#include<utility>
#include"std_grid.hpp"
using namespace better_std;
namespace better_std{
    //row×col 节点的二叉树（以图基类 std_grid 为底层接口，节点编号 0..row*col-1）
    //每个节点至多两个儿子，用 set_left/set_right 显式指定父子关系；
    //left/right/parent 数组提供 O(1) 访问，同时维护图邻接表满足 std_grid 接口。
    //未使用节点（无父也无子）不在树中；不要调用基类 build()。
    template<int row,int col>
    class std_binary_tree : public std_grid<row,col>{
        private:
            std::vector<std::vector<int>> adj;//图接口的无向邻接表
            std::vector<int> left_,right_,par_;//左子/右子/父（-1 表示无）
        public:
            std_binary_tree(){
                this->node_count=row*col;
                adj.assign(this->node_count,std::vector<int>());
                left_.assign(this->node_count,-1);
                right_.assign(this->node_count,-1);
                par_.assign(this->node_count,-1);
            }
            void add_edge(int u,int v) override{//满足图接口：无向加边
                if(u<0||u>=this->node_count||v<0||v>=this->node_count||u==v) return;
                adj[u].push_back(v);
                adj[v].push_back(u);
            }
            //把 v 设为 u 的左儿子；u 的左位空闲且 v 尚无父才成功
            bool set_left(int u,int v){
                if(u<0||u>=this->node_count||v<0||v>=this->node_count||u==v) return false;
                if(left_[u]!=-1||par_[v]!=-1) return false;
                left_[u]=v;par_[v]=u;
                add_edge(u,v);
                return true;
            }
            bool set_right(int u,int v){
                if(u<0||u>=this->node_count||v<0||v>=this->node_count||u==v) return false;
                if(right_[u]!=-1||par_[v]!=-1) return false;
                right_[u]=v;par_[v]=u;
                add_edge(u,v);
                return true;
            }
            int left(int u) const{ return (u>=0&&u<this->node_count)?left_[u]:-1; }
            int right(int u) const{ return (u>=0&&u<this->node_count)?right_[u]:-1; }
            int parent(int u) const{ return (u>=0&&u<this->node_count)?par_[u]:-1; }
            bool is_root(int u) const{ return par_[u]==-1 && !adj[u].empty(); }
            bool is_leaf(int u) const{ return left_[u]==-1 && right_[u]==-1; }
            //根：唯一无父且有边的节点；空树返回 -1
            int root() const{
                for(int u=0;u<this->node_count;u++) if(is_root(u)) return u;
                return -1;
            }
            std::vector<int> children(int u) const{
                std::vector<int> res;
                if(u<0||u>=this->node_count) return res;
                if(left_[u]!=-1) res.push_back(left_[u]);
                if(right_[u]!=-1) res.push_back(right_[u]);
                return res;
            }
            int degree(int node) const override{ return (int)adj[node].size(); }
            const std::vector<int>& operator[](int node) const{ return adj[node]; }
            int operator()(int u,int v) const{ return edge(u,v)?1:0; }//默认边权 1
            bool edge(int u,int v) const{
                for(int x:adj[u]) if(x==v) return true;
                return false;
            }
            void print() const override{
                for(int u=0;u<this->node_count;u++){
                    std::cout<<"node "<<u<<" ("<<(u/col)<<","<<(u%col)<<") l="<<left_[u]
                             <<" r="<<right_[u]<<" p="<<par_[u]<<"\n";
                }
            }
            //===== 二叉树的遍历（均以 root() 为根；空树返回空）=====
            std::vector<int> preorder() const{
                std::vector<int> res,stk;
                int r=root();if(r==-1) return res;
                stk.push_back(r);
                while(!stk.empty()){
                    int u=stk.back();stk.pop_back();
                    res.push_back(u);
                    if(right_[u]!=-1) stk.push_back(right_[u]);//先右后左入栈→左先出
                    if(left_[u]!=-1) stk.push_back(left_[u]);
                }
                return res;
            }
            std::vector<int> inorder() const{
                std::vector<int> res,stk;
                int r=root();if(r==-1) return res;
                int cur=r;
                while(cur!=-1||!stk.empty()){
                    while(cur!=-1){ stk.push_back(cur); cur=left_[cur]; }
                    cur=stk.back();stk.pop_back();
                    res.push_back(cur);
                    cur=right_[cur];
                }
                return res;
            }
            std::vector<int> postorder() const{
                std::vector<int> res,stk;
                int r=root();if(r==-1) return res;
                stk.push_back(r);
                while(!stk.empty()){//先序变体 根-右-左，反转即 左-右-根
                    int u=stk.back();stk.pop_back();
                    res.push_back(u);
                    if(left_[u]!=-1) stk.push_back(left_[u]);
                    if(right_[u]!=-1) stk.push_back(right_[u]);
                }
                std::reverse(res.begin(),res.end());
                return res;
            }
            std::vector<int> levelorder() const{
                std::vector<int> q;
                int r=root();if(r==-1) return q;
                q.push_back(r);
                for(size_t h=0;h<q.size();h++){
                    int u=q[h];
                    for(int v:children(u)) q.push_back(v);
                }
                return q;
            }
            //树高（边数）：空树 -1，单点 0
            int height() const{
                int r=root();if(r==-1) return -1;
                int h=-1;
                std::vector<int> q;q.push_back(r);
                for(size_t head=0;head<q.size();){
                    size_t tail=q.size();
                    for(;head<tail;head++){
                        int u=q[head];
                        for(int v:children(u)) q.push_back(v);
                    }
                    h++;
                }
                return h;
            }
    };
}
