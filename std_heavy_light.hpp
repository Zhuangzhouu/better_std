#pragma once
#include<vector>
#include<utility>
#include<algorithm>
namespace better_std{
    //树的重链剖分（HLD）：把树拆成若干重链，配合线段树做路径/子树查询
    //节点编号 0..row*col-1；构造给边表（无向）与根
    //pos[u] 为剖分后的 dfs 序下标；path_segments 把路径拆成若干 [pos_l,pos_r] 连续区间
    template<int row,int col>
    class std_heavy_light{
        private:
            int n_;
            std::vector<std::vector<int>> adj_;
            std::vector<int> parent_,depth_,sz_,heavy_,head_,pos_,id_;
            int cur_;
        public:
            std_heavy_light(const std::vector<std::pair<int,int>>& edges,int root=0){
                n_=row*col;
                adj_.assign(n_,std::vector<int>());
                for(auto& e:edges){
                    int u=e.first,v=e.second;
                    if(u>=0&&u<n_&&v>=0&&v<n_&&u!=v){ adj_[u].push_back(v); adj_[v].push_back(u); }
                }
                parent_.assign(n_,-1);depth_.assign(n_,0);sz_.assign(n_,1);
                heavy_.assign(n_,-1);head_.assign(n_,-1);
                pos_.assign(n_,-1);id_.assign(n_,-1);
                if(root<0||root>=n_) root=0;
                //1. 迭代求子树大小与重儿子
                std::vector<int> order,stk;
                stk.push_back(root);
                while(!stk.empty()){
                    int u=stk.back();stk.pop_back();
                    order.push_back(u);
                    for(int v:adj_[u]) if(v!=parent_[u]){ parent_[v]=u; depth_[v]=depth_[u]+1; stk.push_back(v); }
                }
                for(int i=(int)order.size()-1;i>=0;i--){
                    int u=order[i];
                    sz_[u]=1;int best=0;heavy_[u]=-1;
                    for(int v:adj_[u]) if(v!=parent_[u]){
                        sz_[u]+=sz_[v];
                        if(sz_[v]>best){ best=sz_[v]; heavy_[u]=v; }
                    }
                }
                //2. 沿重链分配 dfs 序
                cur_=0;
                std::vector<std::pair<int,int>> stk2;
                stk2.push_back({root,root});
                while(!stk2.empty()){
                    int u=stk2.back().first,h=stk2.back().second;
                    stk2.pop_back();
                    while(u!=-1){
                        head_[u]=h;
                        pos_[u]=cur_;id_[cur_]=u;cur_++;
                        for(int v:adj_[u]) if(v!=parent_[u]&&v!=heavy_[u]) stk2.push_back({v,v});
                        u=heavy_[u];
                    }
                }
            }
            int size() const{ return n_; }
            int pos(int u) const{ return pos_[u]; }
            int node_at(int p) const{ return id_[p]; }
            int head(int u) const{ return head_[u]; }
            int parent(int u) const{ return parent_[u]; }
            int depth(int u) const{ return depth_[u]; }
            int subtree_size(int u) const{ return sz_[u]; }
            //路径 u→v 分解为若干 [pos_l,pos_r]（每个区间是某条重链上的连续段）
            std::vector<std::pair<int,int>> path_segments(int u,int v) const{
                std::vector<std::pair<int,int>> segs;
                while(head_[u]!=head_[v]){
                    if(depth_[head_[u]]<depth_[head_[v]]) std::swap(u,v);
                    segs.push_back({pos_[head_[u]],pos_[u]});
                    u=parent_[head_[u]];
                }
                if(depth_[u]>depth_[v]) std::swap(u,v);
                segs.push_back({pos_[u],pos_[v]});
                return segs;
            }
            //重链版 LCA
            int lca(int u,int v) const{
                while(head_[u]!=head_[v]){
                    if(depth_[head_[u]]<depth_[head_[v]]) std::swap(u,v);
                    u=parent_[head_[u]];
                }
                return depth_[u]<depth_[v]?u:v;
            }
            //路径上的节点集合（顺序不定；用区间展开，便于暴力对拍）
            std::vector<int> path_nodes(int u,int v) const{
                std::vector<int> res;
                auto segs=path_segments(u,v);
                for(auto& s:segs)
                    for(int p=s.first;p<=s.second;p++) res.push_back(id_[p]);
                return res;
            }
    };
}
