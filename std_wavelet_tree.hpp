#ifndef BETTER_STD_STD_WAVELET_TREE_HPP
#define BETTER_STD_STD_WAVELET_TREE_HPP

//===== better_std 小波树（Wavelet Tree）=====
//   支持 O(log σ) 的区间查询：第 k 小、区间 ≤x 计数、区间 <x 计数、区间排名。
//   值域自动压缩（映射到 [0,σ)）；构造 O(n log σ)。
//   API：build(a)、kth(l,r,k)（0-based 闭区间，k=0 起）、count_leq(l,r,x)、rank(l,r,x)。
#include<vector>
#include<algorithm>
#include<cstddef>
namespace better_std{

template<typename T>
class wavelet_tree{
    struct node{
        int lo,hi;                 // 值域 [lo,hi]
        std::vector<int> b;        // b[i] = 前缀 0 的个数
        int l,r;
        node(int lo_,int hi_,const std::vector<int>& b_):lo(lo_),hi(hi_),b(b_),l(-1),r(-1){}
    };
    std::vector<node> tr;
    std::vector<T> vals_;          // 压缩值 → 原值
    int n_, root_=-1;

    int build_(const std::vector<int>& a,int l,int r,int lo,int hi){
        if(l>=r) return -1;
        int id=(int)tr.size();
        tr.push_back(node(lo,hi,std::vector<int>(r-l+1,0)));
        if(lo>=hi) return id;      // 叶子：值域已单点，不再细分
        int mid=(lo+hi)/2;
        // 稳定划分：0 放左，1 放右
        std::vector<int> left,right;
        left.reserve(r-l); right.reserve(r-l);
        for(int i=l;i<r;i++){
            if(a[i]<=mid){ left.push_back(a[i]); tr[id].b[i-l+1]=tr[id].b[i-l]+1; }
            else { right.push_back(a[i]); tr[id].b[i-l+1]=tr[id].b[i-l]; }
        }
        // 先算子树索引再赋值（避免 push_back 重分配导致求值顺序 UB）
        int lc=build_(left,0,(int)left.size(),lo,mid);
        int rc=build_(right,0,(int)right.size(),mid+1,hi);
        tr[id].l=lc; tr[id].r=rc;
        return id;
    }
    // 区间 [l,r) 中 <= x 的个数
    int leq_(int u,int l,int r,int x) const {
        if(u<0||l>=r) return 0;
        if(x<tr[u].lo) return 0;
        if(x>=tr[u].hi) return r-l;
        int mid=(tr[u].lo+tr[u].hi)/2;
        int lb=tr[u].b[l], rb=tr[u].b[r];       // 左半在 [l,r) 的个数
        if(x<=mid) return leq_(tr[u].l,lb,rb,x);
        return rb-lb+leq_(tr[u].r,l-lb,r-(rb),x);
    }
    // 第 k 小（0-based），沿 b 前缀下溯到值域单点
    T kth_(int u,int l,int r,int k) const {
        if(u<0) return T();
        if(tr[u].lo>=tr[u].hi) return vals_[(size_t)tr[u].lo];
        int mid=(tr[u].lo+tr[u].hi)/2;
        int lb=tr[u].b[l], rb=tr[u].b[r];       // 左半在 [l,r) 的元素数
        int cnt0=rb-lb;
        if(k<cnt0) return kth_(tr[u].l,lb,rb,k);
        return kth_(tr[u].r,l-lb,r-rb,k-cnt0);
    }
public:
    wavelet_tree() {}
    explicit wavelet_tree(const std::vector<T>& a){ build(a); }
    void build(const std::vector<T>& a){
        tr.clear(); vals_.clear(); n_=(int)a.size(); root_=-1;
        if(n_==0) return;
        vals_=a;
        std::sort(vals_.begin(),vals_.end());
        vals_.erase(std::unique(vals_.begin(),vals_.end()),vals_.end());
        std::vector<int> comp(n_);
        for(int i=0;i<n_;i++) comp[i]=(int)(std::lower_bound(vals_.begin(),vals_.end(),a[i])-vals_.begin());
        root_=build_(comp,0,n_,0,(int)vals_.size()-1);
    }
    bool empty() const { return n_==0; }
    int size() const { return n_; }
    // 闭区间 [l,r] 第 k 小（k=0 起）
    T kth(int l,int r,int k) const {
        if(l>r||l<0||r>=n_||k<0||k>r-l+1) return T();
        return kth_(root_,l,r+1,k);
    }
    // 闭区间 [l,r] 中 <= x 的个数
    int count_leq(int l,int r,const T& x) const {
        if(l>r||l<0||r>=n_) return 0;
        auto it=std::upper_bound(vals_.begin(),vals_.end(),x);
        int cx=(int)(it-vals_.begin())-1;
        if(cx<0) return 0;
        if(cx>=(int)vals_.size()-1) return r-l+1;
        return leq_(root_,l,r+1,cx);
    }
    int count_less(int l,int r,const T& x) const {
        auto it=std::lower_bound(vals_.begin(),vals_.end(),x);
        int cx=(int)(it-vals_.begin())-1;
        if(cx<0) return 0;
        return leq_(root_,l,r+1,cx);
    }
};

} // namespace better_std
#endif // BETTER_STD_STD_WAVELET_TREE_HPP
