#ifndef BETTER_STD_STD_TREAP_HPP
#define BETTER_STD_STD_TREAP_HPP

#include<vector>
#include<random>
#include<algorithm>
#include<limits>
namespace better_std{
    //===== FHQ Treap（无旋 Treap）=====
    //  集合语义：按 key 的 multiset，支持 insert / erase_one / rank / kth / 前驱 / 后继
    //  序列语义：按下标的区间翻转（懒标记），适合文艺平衡树
    //  所有操作 O(log n) 期望；节点下标 0 保留为 null
    template<typename T=long long>
    class fhq_treap{
        struct Node{
            T key{};
            unsigned pri=0;
            int l=0,r=0,sz=1;
            bool rev=false;
        };
        std::vector<Node> tr;
        int root_=0;
        std::mt19937 rng_;
        int newnode(const T& k){
            Node nd; nd.key=k; nd.pri=rng_(); nd.l=nd.r=0; nd.sz=1; nd.rev=false;
            tr.push_back(nd);
            return (int)tr.size()-1;
        }
        int sz(int x)const{ return x?tr[x].sz:0; }
        void pull(int x){ if(x) tr[x].sz=sz(tr[x].l)+sz(tr[x].r)+1; }
        void apply_rev(int x){ if(x){ tr[x].rev=!tr[x].rev; std::swap(tr[x].l,tr[x].r); } }
        void push(int x){
            if(x&&tr[x].rev){ apply_rev(tr[x].l); apply_rev(tr[x].r); tr[x].rev=false; }
        }
        int merge(int a,int b){
            if(!a||!b) return a?a:b;
            if(tr[a].pri>tr[b].pri){ push(a); tr[a].r=merge(tr[a].r,b); pull(a); return a; }
            push(b); tr[b].l=merge(a,tr[b].l); pull(b); return b;
        }
        //a: key<=key，b: key>key
        void split_key(int x,const T& key,int& a,int& b){
            if(!x){ a=b=0; return; }
            push(x);
            if(!(key<tr[x].key)){ split_key(tr[x].r,key,tr[x].r,b); a=x; pull(a); }
            else { split_key(tr[x].l,key,a,tr[x].l); b=x; pull(b); }
        }
        //a: key<key，b: key>=key
        void split_lt(int x,const T& key,int& a,int& b){
            if(!x){ a=b=0; return; }
            push(x);
            if(tr[x].key<key){ split_lt(tr[x].r,key,tr[x].r,b); a=x; pull(a); }
            else { split_lt(tr[x].l,key,a,tr[x].l); b=x; pull(b); }
        }
        //a: 前 k 个元素（按下标），b: 其余
        void split_size(int x,int k,int& a,int& b){
            if(!x){ a=b=0; return; }
            push(x);
            if(sz(tr[x].l)>=k){ split_size(tr[x].l,k,a,tr[x].l); b=x; pull(b); }
            else { split_size(tr[x].r,k-sz(tr[x].l)-1,tr[x].r,b); a=x; pull(a); }
        }
        void collect(int x,std::vector<T>& out)const{
            if(!x) return;
            const_cast<fhq_treap*>(this)->push(x);
            collect(tr[x].l,out); out.push_back(tr[x].key); collect(tr[x].r,out);
        }
    public:
        explicit fhq_treap(unsigned seed=20261104U):tr(1),rng_(seed){}
        int size()const{ return sz(root_); }
        bool empty()const{ return root_==0; }
        void clear(){ tr.assign(1,Node()); root_=0; }
        void insert(const T& key){
            int a=0,b=0; split_key(root_,key,a,b);
            root_=merge(merge(a,newnode(key)),b);
        }
        bool erase_one(const T& key){
            int a=0,b=0,mid=0,c=0;
            split_lt(root_,key,a,b);
            split_key(b,key,mid,c);
            if(!mid){ root_=merge(a,c); return false; }
            int rest=merge(tr[mid].l,tr[mid].r);
            root_=merge(merge(a,rest),c);
            return true;
        }
        bool contains(const T& key)const{
            int x=root_;
            while(x){
                if(tr[x].key==key) return true;
                x=(key<tr[x].key)?tr[x].l:tr[x].r;
            }
            return false;
        }
        //小于 key 的元素个数
        int rank_of(const T& key){
            int a=0,b=0; split_lt(root_,key,a,b);
            int r=sz(a); root_=merge(a,b); return r;
        }
        int count(const T& key){
            int a=0,b=0; split_lt(root_,key,a,b);
            int mid=0,c=0; split_key(b,key,mid,c);
            int r=sz(mid); root_=merge(merge(a,mid),c); return r;
        }
        //第 k 小（1-based）；越界返回 T 的默认值
        T kth(int k){
            if(k<1||k>sz(root_)) return T{};
            int x=root_;
            while(x){
                push(x);
                int ls=sz(tr[x].l);
                if(k<=ls) x=tr[x].l;
                else if(k==ls+1) return tr[x].key;
                else { k-=ls+1; x=tr[x].r; }
            }
            return T{};
        }
        //严格小于 key 的最大值；不存在返回 false
        bool predecessor(const T& key,T& out){
            int a=0,b=0; split_lt(root_,key,a,b);
            bool ok=a!=0;
            if(ok){ int x=a; while(true){ push(x); if(tr[x].r) x=tr[x].r; else break; } out=tr[x].key; }
            root_=merge(a,b); return ok;
        }
        //严格大于 key 的最小值；不存在返回 false
        bool successor(const T& key,T& out){
            int a=0,b=0; split_key(root_,key,a,b);
            bool ok=b!=0;
            if(ok){ int x=b; while(true){ push(x); if(tr[x].l) x=tr[x].l; else break; } out=tr[x].key; }
            root_=merge(a,b); return ok;
        }
        std::vector<T> to_vector()const{ std::vector<T> v; v.reserve(size()); collect(root_,v); return v; }
        //序列语义：按 v 的顺序建树（中序=v），用于区间翻转等文艺平衡树操作
        void build(const std::vector<T>& v){ clear(); for(const T& x:v) root_=merge(root_,newnode(x)); }
        //集合语义：按 key 插入（中序=v 的有序排列），用于 rank/kth/前驱后继
        void build_set(const std::vector<T>& v){ clear(); for(const T& x:v) insert(x); }
        //注意：序列模式（build + reverse_range）与集合模式（insert/split_key）不要混用，
        //翻转后树中序不再按 key 有序，此时调用 insert/rank_of 等按 key 的操作无意义。
        //序列翻转 [l,r]（0-based 闭区间）
        void reverse_range(int l,int r){
            if(l>r||l<0||r>=size()) return;
            int a=0,b=0,c=0;
            split_size(root_,r+1,a,c);
            split_size(a,l,a,b);
            apply_rev(b);
            root_=merge(merge(a,b),c);
        }
    };
}

#endif // BETTER_STD_STD_TREAP_HPP
