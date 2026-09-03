#pragma once
#include<iostream>
#include<vector>
#include<utility>
#include<algorithm>
#include"std_rootless_tree.hpp"
using namespace better_std;
namespace better_std{
    //row×col 节点的有根树（以无根树 std_rootless_tree 为底层，即最终以图 std_grid 为底层接口）
    //在构造/reroot 时固定一个根，之后 parent/depth/subtree_size 均以该根为方向；
    //children(u) 返回 u 的儿子列表，height()/bfs_order()/dfs_order() 以根为起点。
    //注意：不要调用基类 build()；add_edge 后会自动按当前根重建缓存。
    template<int row,int col>
    class std_tree : public std_rootless_tree<row,col>{
        private:
            int root_;
        public:
            std_tree(int root=0): std_rootless_tree<row,col>(){
                if(root<0||root>=this->size()) root=0;
                root_=root;
                this->root_at(root_);
            }
            std_tree(const std::vector<std::pair<int,int>>& edges,int root=0)
                : std_rootless_tree<row,col>(edges){
                if(root<0||root>=this->size()) root=0;
                root_=root;
                this->root_at(root_);
            }
            //加边后自动按当前根重建 parent/depth/subtree 缓存
            void add_edge(int u,int v) override{
                std_rootless_tree<row,col>::add_edge(u,v);
                this->root_at(root_);
            }
            int root() const{ return root_; }
            //换根
            void reroot(int r){
                if(r<0||r>=this->size()) r=0;
                root_=r;
                this->root_at(r);
            }
            //以根为方向求儿子列表
            std::vector<int> children(int u) const{
                std::vector<int> res;
                if(u<0||u>=this->size()) return res;
                for(int v:(*this)[u]) if(this->parent(v)==u) res.push_back(v);
                return res;
            }
            bool is_leaf(int u) const{ return children(u).empty(); }
            //树高 = 距根最远的深度（边数）
            int height() const{
                int h=0;
                for(int u=0;u<this->size();u++) h=std::max(h,this->depth(u));
                return h;
            }
            //按层序 BFS 得到节点序列
            std::vector<int> bfs_order() const{
                std::vector<int> q;
                q.push_back(root_);
                for(size_t h=0;h<q.size();h++){
                    int u=q[h];
                    for(int v:children(u)) q.push_back(v);
                }
                return q;
            }
            //前序 DFS 节点序列（先父后子）
            std::vector<int> dfs_order() const{
                std::vector<int> res,stk;
                stk.push_back(root_);
                while(!stk.empty()){
                    int u=stk.back();stk.pop_back();
                    res.push_back(u);
                    auto ch=children(u);
                    for(int i=(int)ch.size()-1;i>=0;i--) stk.push_back(ch[i]);//后进先出→左先出
                }
                return res;
            }
    };
}
