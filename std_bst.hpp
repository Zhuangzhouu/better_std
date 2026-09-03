#pragma once
#include<iostream>
#include<vector>
#include<utility>
namespace better_std{
    //普通二叉搜索树（动态节点，元素类型 T 需支持 < 与 ==）
    //插入/删除/查找均为 O(树高)；有序插入会退化为链，需要平衡可换 AVL/treap
    //维护子树大小：kth(k) 第 k 小、rank(k) 严格小于 k 的个数、count_range
    template<typename T>
    class std_bst{
        private:
            struct Node{ T key; int l=-1,r=-1,sz=1; };//sz=子树节点数
            std::vector<Node> pool;
            int root_=-1,cnt_=0;
            int new_node(const T& k){ pool.push_back({k,-1,-1,1}); return (int)pool.size()-1; }
            int lsz(int u) const{ return pool[u].l==-1?0:pool[pool[u].l].sz; }
            bool find_rec(int u,const T& k) const{
                if(u==-1) return false;
                if(k<pool[u].key) return find_rec(pool[u].l,k);
                if(pool[u].key<k) return find_rec(pool[u].r,k);
                return true;
            }
            void inorder_rec(int u,std::vector<T>& out) const{
                if(u==-1) return;
                inorder_rec(pool[u].l,out);
                out.push_back(pool[u].key);
                inorder_rec(pool[u].r,out);
            }
            //递归删除（不 push_back，引用安全）；删除成功时沿路径维护 sz
            bool erase_rec(int& u,const T& k){
                if(u==-1) return false;
                if(k<pool[u].key){
                    if(erase_rec(pool[u].l,k)){ pool[u].sz--; return true; }
                    return false;
                }
                if(pool[u].key<k){
                    if(erase_rec(pool[u].r,k)){ pool[u].sz--; return true; }
                    return false;
                }
                if(pool[u].l==-1){ u=pool[u].r; cnt_--; return true; }
                if(pool[u].r==-1){ u=pool[u].l; cnt_--; return true; }
                //两孩子：右子树最小节点顶替，再递归删掉该最小节点
                int s=pool[u].r;
                while(pool[s].l!=-1) s=pool[s].l;
                pool[u].key=pool[s].key;
                erase_rec(pool[u].r,pool[s].key);
                pool[u].sz--;
                return true;
            }
        public:
            std_bst(){}
            bool insert(const T& k){
                if(root_==-1){ root_=new_node(k); cnt_++; return true; }
                int u=root_;
                while(true){
                    pool[u].sz++;
                    if(k<pool[u].key){
                        if(pool[u].l==-1){ pool[u].l=new_node(k); cnt_++; return true; }
                        u=pool[u].l;
                    }else if(pool[u].key<k){
                        if(pool[u].r==-1){ pool[u].r=new_node(k); cnt_++; return true; }
                        u=pool[u].r;
                    }else{ pool[u].sz--; return false; }//重复键：回退 sz
                }
            }
            bool erase(const T& k){ return erase_rec(root_,k); }
            bool contains(const T& k) const{ return find_rec(root_,k); }
            bool empty() const{ return root_==-1; }
            int size() const{ return cnt_; }
            std::vector<T> inorder() const{
                std::vector<T> out;
                inorder_rec(root_,out);
                return out;
            }
            //最小/最大键（空树未定义，调用前先判 empty）
            T min() const{
                int u=root_;
                while(pool[u].l!=-1) u=pool[u].l;
                return pool[u].key;
            }
            T max() const{
                int u=root_;
                while(pool[u].r!=-1) u=pool[u].r;
                return pool[u].key;
            }
            //第 k 小（1-based，1<=k<=size()）
            T kth(int k) const{
                int u=root_;
                while(u!=-1){
                    int l=lsz(u);
                    if(k<=l){ u=pool[u].l; }
                    else if(k==l+1){ return pool[u].key; }
                    else{ k-=l+1; u=pool[u].r; }
                }
                return T();//不会到达
            }
            //严格小于 k 的元素个数
            int rank(const T& k) const{
                int u=root_,res=0;
                while(u!=-1){
                    if(pool[u].key<k){ res+=lsz(u)+1; u=pool[u].r; }
                    else u=pool[u].l;
                }
                return res;
            }
            //半开区间 [lo,hi) 内的元素个数
            int count_range(const T& lo,const T& hi) const{ return rank(hi)-rank(lo); }
            //小于 k 的最大键；不存在返回 {false,T()}
            std::pair<bool,T> predecessor(const T& k) const{
                int u=root_,best=-1;
                while(u!=-1){
                    if(pool[u].key<k){ best=u; u=pool[u].r; }
                    else u=pool[u].l;
                }
                if(best==-1) return {false,T()};
                return {true,pool[best].key};
            }
            //大于 k 的最小键；不存在返回 {false,T()}
            std::pair<bool,T> successor(const T& k) const{
                int u=root_,best=-1;
                while(u!=-1){
                    if(pool[u].key>k){ best=u; u=pool[u].l; }
                    else u=pool[u].r;
                }
                if(best==-1) return {false,T()};
                return {true,pool[best].key};
            }
    };
}
