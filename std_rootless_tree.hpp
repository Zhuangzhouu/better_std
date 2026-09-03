#pragma once
#include<iostream>
#include<vector>
#include<utility>
#include<algorithm>
#include"std_grid.hpp"
using namespace better_std;
namespace better_std{
    //row×col 节点的无根树（以图基类 std_grid 为底层接口）
    //节点编号 0..row*col-1 与网格图一致（node_id/coord/size/degree/print 等接口完全复用，
    //可通过 std_grid<row,col>* 多态访问）；边的拓扑由用户 add_edge 指定，不要求是网格相邻边。
    //只要连通且恰有 n-1 条边，就是一棵无根树。
    //注意：不要调用基类 build()（会把所有网格邻接边都加上，必然成环）。
    template<int row,int col>
    class std_rootless_tree : public std_grid<row,col>{
        private:
            std::vector<std::vector<int>> adj;//无向邻接表
            mutable std::vector<int> par_,dep_,sub_;//root_at 的缓存：父/深度/子树大小
            mutable std::vector<std::vector<int>> up_;//倍增表 up_[k][u]：u 的 2^k 级祖先
            mutable int LOG_=0,root_=-1;
        public:
            std_rootless_tree(){
                this->node_count=row*col;
                adj.assign(this->node_count,std::vector<int>());
            }
            //直接给定边表建树（无向）
            std_rootless_tree(const std::vector<std::pair<int,int>>& edges){
                this->node_count=row*col;
                adj.assign(this->node_count,std::vector<int>());
                for(auto& e:edges) add_edge(e.first,e.second);
            }
            void add_edge(int u,int v) override{
                if(u<0||u>=this->node_count||v<0||v>=this->node_count||u==v) return;
                adj[u].push_back(v);
                adj[v].push_back(u);
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
            //===== 无根树特性 =====
            int edge_count() const{
                long long e=0;
                for(int u=0;u<this->node_count;u++) e+=adj[u].size();
                return (int)(e/2);
            }
            //连通且边数恰为 n-1（隐含无自环；重边会使边数超限或不连通）即为一棵树
            bool is_tree() const{
                int n=this->node_count;
                if(edge_count()!=n-1) return false;
                std::vector<int> vis(n,0),q;
                q.push_back(0);vis[0]=1;
                int cnt=1;
                for(size_t h=0;h<q.size();h++){
                    int u=q[h];
                    for(int v:adj[u]) if(!vis[v]){vis[v]=1;cnt++;q.push_back(v);}
                }
                return cnt==n;
            }
            //任选一点为根做 DFS（迭代），缓存 parent/depth/subtree_size
            void root_at(int root) const{
                int n=this->node_count;
                if(root<0||root>=n) root=0;
                par_.assign(n,-2);dep_.assign(n,0);sub_.assign(n,1);
                std::vector<int> post;post.reserve(n);
                std::vector<std::pair<int,int>> stk;//(node,下一待访问邻居下标)
                par_[root]=-1;stk.push_back({root,0});
                while(!stk.empty()){
                    int u=stk.back().first;
                    int& idx=stk.back().second;
                    if(idx<(int)adj[u].size()){
                        int v=adj[u][idx++];
                        if(par_[v]!=-2) continue;//已访问（含父）跳过；对环也安全
                        par_[v]=u;dep_[v]=dep_[u]+1;stk.push_back({v,0});
                    }else{
                        post.push_back(u);stk.pop_back();
                    }
                }
                for(int u: post){//前序 post：孩子先于父出现，累加到父
                    if(par_[u]>=0) sub_[par_[u]]+=sub_[u];
                }
                //构建倍增表
                LOG_=1;
                while((1LL<<LOG_)<(long long)n) LOG_++;
                up_.assign(LOG_,std::vector<int>(n,-1));
                for(int u=0;u<n;u++) up_[0][u]=(par_[u]>=0?par_[u]:-1);//根/未访问为 -1
                for(int k=1;k<LOG_;k++)
                    for(int u=0;u<n;u++){
                        int mid=up_[k-1][u];
                        up_[k][u]=(mid==-1?-1:up_[k-1][mid]);
                    }
                root_=root;
            }
            int root() const{ return root_; }
            int parent(int u) const{
                if((int)par_.size()!=this->node_count) root_at(0);
                return (u>=0&&u<this->node_count)?par_[u]:-2;
            }
            int depth(int u) const{
                if((int)par_.size()!=this->node_count) root_at(0);
                return (u>=0&&u<this->node_count)?dep_[u]:-1;
            }
            int subtree_size(int u) const{
                if((int)par_.size()!=this->node_count) root_at(0);
                return (u>=0&&u<this->node_count)?sub_[u]:0;
            }
            //===== LCA（倍增；需先 root_at，root_at 会自动建倍增表）=====
            //u 向上跳 k 步的祖先；超出根/不连通返回 -1
            int kth_ancestor(int u,int k) const{
                if((int)up_.size()==0||(int)up_[0].size()!=this->node_count) root_at(0);
                for(int b=0;b<LOG_&&k>0&&u!=-1;b++){
                    if(k&1) u=up_[b][u];
                    k>>=1;
                }
                return u;
            }
            int lca(int u,int v) const{
                if((int)up_.size()==0||(int)up_[0].size()!=this->node_count) root_at(0);
                if(u<0||u>=this->node_count||v<0||v>=this->node_count) return -1;
                if(dep_[u]<0||dep_[v]<0) return -1;//不在当前根所在连通块
                if(dep_[u]<dep_[v]) std::swap(u,v);
                u=kth_ancestor(u,dep_[u]-dep_[v]);
                if(u==v) return u;
                for(int b=LOG_-1;b>=0;b--)
                    if(up_[b][u]!=up_[b][v]){ u=up_[b][u]; v=up_[b][v]; }
                return up_[0][u];
            }
            //树上距离（边数）；不连通返回 -1
            int dist(int u,int v) const{
                int w=lca(u,v);
                if(w==-1) return -1;
                return dep_[u]+dep_[v]-2*dep_[w];
            }
            //从 src BFS 返回最远节点，dist 存距离
            int bfs_far(int src,std::vector<int>& dist) const{
                int n=this->node_count;
                dist.assign(n,-1);
                std::vector<int> q;q.push_back(src);dist[src]=0;
                int far=src;
                for(size_t h=0;h<q.size();h++){
                    int u=q[h];
                    if(dist[u]>dist[far]) far=u;
                    for(int v:adj[u]) if(dist[v]<0){dist[v]=dist[u]+1;q.push_back(v);}
                }
                return far;
            }
            //直径：两次 BFS，返回两个端点
            std::pair<int,int> diameter() const{
                std::vector<int> d;
                int a=bfs_far(0,d);
                int b=bfs_far(a,d);
                return {a,b};
            }
            int diameter_length() const{
                std::vector<int> d;
                int a=bfs_far(0,d);
                int b=bfs_far(a,d);
                return d[b];
            }
            //重心：删掉后剩余最大连通块最小的节点（并列取编号小者）
            int centroid() const{
                if((int)par_.size()!=this->node_count) root_at(0);
                int n=this->node_count;
                int best=-1,best_max=n+1;
                for(int u=0;u<n;u++){
                    int mx=n-sub_[u];//父侧子树
                    for(int v:adj[u]) if(par_[v]==u) mx=std::max(mx,sub_[v]);
                    if(mx<best_max){best_max=mx;best=u;}
                }
                return best;
            }
            //u 到 v 的简单路径（节点序列，含两端）；不连通返回空
            std::vector<int> path(int u,int v) const{
                std::vector<int> res;
                if(u<0||u>=this->node_count||v<0||v>=this->node_count) return res;
                root_at(u);
                if(par_[v]==-2) return res;//不在同一连通块
                int x=v;
                while(x>=0){ res.push_back(x); if(x==u) break; x=par_[x]; }
                std::reverse(res.begin(),res.end());
                return res;
            }
    };
}
