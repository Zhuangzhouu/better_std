#ifndef BETTER_STD_STD_MERGEABLE_HEAP_HPP
#define BETTER_STD_STD_MERGEABLE_HEAP_HPP

//===== better_std 可并堆三件套：左偏树 / 斜堆 / 配对堆 =====
//   均支持 merge / push / top / pop / empty / size，泛型比较器（默认小根）。
//   · leftist_heap：O(log n) merge，最坏保证，基于 dist(最短右路径)。
//   · skew_heap：摊还 O(log n)，交换左右孩子（无 dist 维护）。
//   · pairing_heap：摊还 O(log n)，两趟合并，代码最短。
#include<vector>
#include<functional>
#include<cstddef>
namespace better_std{

//—— 左偏树（小根，可并）——
template<typename T,typename Compare=std::less<T>>
class leftist_heap{
    struct node{ T val; int l,r,dist; node(const T& v):val(v),l(-1),r(-1),dist(0){} };
    std::vector<node> tr;
    Compare cmp_;
    int root_=-1;
    int merge_(int a,int b){
        if(a<0) return b;
        if(b<0) return a;
        if(cmp_(tr[b].val,tr[a].val)) std::swap(a,b);   // a 为较小根
        tr[a].r=merge_(tr[a].r,b);
        int dl=tr[a].l<0?0:tr[tr[a].l].dist;
        int dr=tr[a].r<0?0:tr[tr[a].r].dist;
        if(dl<dr) std::swap(tr[a].l,tr[a].r);       // 左偏：左 dist >= 右
        tr[a].dist=dr<dl?dr+1:dl+1;                 // dist = 右孩子 dist+1
        return a;
    }
public:
    leftist_heap() {}
    explicit leftist_heap(Compare c):cmp_(c) {}
    bool empty() const { return root_<0; }
    size_t size() const { return tr.size(); }
    void clear(){ tr.clear(); root_=-1; }
    void push(const T& v){ tr.push_back(node(v)); root_=merge_(root_,(int)tr.size()-1); }
    T top() const { return tr[root_].val; }
    void pop(){
        int l=tr[root_].l,r=tr[root_].r;
        root_=merge_(l,r);
    }
    void merge(leftist_heap& o){
        if(o.root_<0) return;
        // 注意：两堆各有独立 vector，不能直接用索引 merge；逐个节点重新插入
        std::vector<T> vals;
        o.collect_(o.root_,vals);
        for(const T& v:vals) push(v);
        o.clear();
    }
private:
    void collect_(int u,std::vector<T>& out) const {
        if(u<0) return;
        out.push_back(tr[u].val);
        collect_(tr[u].l,out); collect_(tr[u].r,out);
    }
public:
};

//—— 斜堆（skew heap，摊还 O(log n)）——
template<typename T,typename Compare=std::less<T>>
class skew_heap{
    struct node{ T val; int l,r; node(const T& v):val(v),l(-1),r(-1){} };
    std::vector<node> tr;
    Compare cmp_;
    int root_=-1;
    int merge_(int a,int b){
        if(a<0) return b;
        if(b<0) return a;
        if(cmp_(tr[b].val,tr[a].val)) std::swap(a,b);
        tr[a].r=merge_(tr[a].r,b);
        std::swap(tr[a].l,tr[a].r);            // 无条件交换
        return a;
    }
public:
    skew_heap() {}
    explicit skew_heap(Compare c):cmp_(c) {}
    bool empty() const { return root_<0; }
    size_t size() const { return tr.size(); }
    void clear(){ tr.clear(); root_=-1; }
    void push(const T& v){ tr.push_back(node(v)); root_=merge_(root_,(int)tr.size()-1); }
    T top() const { return tr[root_].val; }
    void pop(){ root_=merge_(tr[root_].l,tr[root_].r); }
    void merge(skew_heap& o){
        if(o.root_<0) return;
        std::vector<T> vals;
        o.collect_(o.root_,vals);
        for(const T& v:vals) push(v);
        o.clear();
    }
private:
    void collect_(int u,std::vector<T>& out) const {
        if(u<0) return;
        out.push_back(tr[u].val);
        collect_(tr[u].l,out); collect_(tr[u].r,out);
    }
public:
};

//—— 配对堆（pairing heap，摊还 O(log n)）——
template<typename T,typename Compare=std::less<T>>
class pairing_heap{
    struct node{ T val; std::vector<int> child; node(const T& v):val(v){} };
    std::vector<node> tr;
    Compare cmp_;
    int root_=-1;
    int merge_(int a,int b){
        if(a<0) return b;
        if(b<0) return a;
        if(cmp_(tr[b].val,tr[a].val)) std::swap(a,b);  // a 较小
        tr[a].child.push_back(b);
        return a;
    }
public:
    pairing_heap() {}
    explicit pairing_heap(Compare c):cmp_(c) {}
    bool empty() const { return root_<0; }
    size_t size() const { return tr.size(); }
    void clear(){ tr.clear(); root_=-1; }
    void push(const T& v){ tr.push_back(node(v)); root_=merge_(root_,(int)tr.size()-1); }
    T top() const { return tr[root_].val; }
    void pop(){
        std::vector<int> child=tr[root_].child;   // 先保存
        tr[root_].child.clear();
        int nr=-1;
        // 两趟合并
        std::vector<int> first;
        for(size_t i=0;i<child.size();i+=2){
            int a=child[i], b=(i+1<child.size()?child[i+1]:-1);
            first.push_back(merge_(a,b));
        }
        for(int i=(int)first.size()-1;i>=0;i--) nr=merge_(nr,first[i]);
        root_=nr;
    }
    void merge(pairing_heap& o){
        if(o.root_<0) return;
        std::vector<T> vals;
        o.collect_(o.root_,vals);
        for(const T& v:vals) push(v);
        o.clear();
    }
private:
    void collect_(int u,std::vector<T>& out) const {
        if(u<0) return;
        out.push_back(tr[u].val);
        for(int c:tr[u].child) collect_(c,out);
    }
public:
};

} // namespace better_std
#endif // BETTER_STD_STD_MERGEABLE_HEAP_HPP
