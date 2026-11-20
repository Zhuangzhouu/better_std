#ifndef BETTER_STD_STD_LCT_HPP
#define BETTER_STD_STD_LCT_HPP

//===== better_std Link-Cut Tree（动态树）=====
//   维护森林，支持：link/cut、makeroot、路径查询（和/最值）、路径赋值翻转标记。
//   0-based 节点 0..n-1；默认维护点权（val）。
#include<vector>
#include<algorithm>
#include<limits>
namespace better_std{

template<typename T>
class link_cut_tree{
    std::vector<T> val_, mx_;
    std::vector<T> add_;          // 路径加懒标记
    std::vector<char> rev_;       // 翻转懒标记
    std::vector<int> ch_[2], fa_;
    int n_;

    bool is_root(int x) const{
        int f=fa_[x];
        return f<0 || (ch_[0][f]!=x && ch_[1][f]!=x);
    }
    void push_up(int x){
        mx_[x]=val_[x];
        if(ch_[0][x]>=0) mx_[x]=std::max(mx_[x],mx_[ch_[0][x]]);
        if(ch_[1][x]>=0) mx_[x]=std::max(mx_[x],mx_[ch_[1][x]]);
    }
    void apply_rev(int x){ if(x<0) return; std::swap(ch_[0][x],ch_[1][x]); rev_[x]^=1; }
    void apply_add(int x,T v){ if(x<0) return; val_[x]+=v; mx_[x]+=v; add_[x]+=v; }
    void push_down(int x){
        if(rev_[x]){ apply_rev(ch_[0][x]); apply_rev(ch_[1][x]); rev_[x]=0; }
        if(add_[x]!=T(0)){ apply_add(ch_[0][x],add_[x]); apply_add(ch_[1][x],add_[x]); add_[x]=T(0); }
    }
    void push_all(int x){
        if(!is_root(x)) push_all(fa_[x]);
        push_down(x);
    }
    void rotate(int x){
        int y=fa_[x], z=fa_[y];
        int k=(ch_[1][y]==x);
        int b=ch_[k^1][x];
        if(!is_root(y)) ch_[ch_[1][z]==y][z]=x;
        fa_[x]=z;
        ch_[k^1][x]=y; fa_[y]=x;
        ch_[k][y]=b; if(b>=0) fa_[b]=y;
        push_up(y); push_up(x);
    }
    void splay(int x){
        push_all(x);
        while(!is_root(x)){
            int y=fa_[x], z=fa_[y];
            if(!is_root(y)) ((ch_[0][y]==x) ^ (ch_[0][z]==y)) ? rotate(x) : rotate(y);
            rotate(x);
        }
    }
    void access(int x){
        for(int y=-1;x>=0;y=x,x=fa_[x]){
            splay(x); ch_[1][x]=y; if(y>=0) fa_[y]=x; push_up(x);
        }
    }
public:
    link_cut_tree() {}
    explicit link_cut_tree(int n,int val=0){ init(n,val); }
    void init(int n,int val=0){
        n_=n;
        val_.assign(n,T(val)); mx_.assign(n,T(val));
        add_.assign(n,T(0)); rev_.assign(n,0);
        ch_[0].assign(n,-1); ch_[1].assign(n,-1); fa_.assign(n,-1);
    }
    int size() const { return n_; }
    void set_val(int x,T v){ splay(x); val_[x]=v; push_up(x); }
    T get_val(int x){ splay(x); return val_[x]; }
    void makeroot(int x){ access(x); splay(x); apply_rev(x); }
    int findroot(int x){
        access(x); splay(x);
        push_down(x);
        while(ch_[0][x]>=0){ x=ch_[0][x]; push_down(x); }
        splay(x); return x;
    }
    bool connected(int x,int y){
        if(x==y) return true;
        makeroot(x); return findroot(y)==x;   // makeroot 后 x 为根，y 的根等于 x 即连通
    }
    void link(int x,int y){
        makeroot(x);
        if(findroot(y)!=x) fa_[x]=y;
    }
    void cut(int x,int y){
        makeroot(x); access(y); splay(y);
        if(ch_[0][y]==x && ch_[1][x]<0){ ch_[0][y]=-1; fa_[x]=-1; push_up(y); }
    }
    // 路径 x--y 加 v
    void path_add(int x,int y,T v){ makeroot(x); access(y); splay(y); apply_add(y,v); }
    // 路径 x--y 最大点权
    T path_max(int x,int y){ makeroot(x); access(y); splay(y); return mx_[y]; }
    // 子树（以 x 为根，需先 makeroot(x) 语义）最大值：makeroot 后 access+splay，取左子树
    T subtree_max(int x){
        makeroot(x); access(x); splay(x);
        int l=ch_[0][x];
        return l<0 ? val_[x] : mx_[l];   // 左子树为空则只有根自身
    }
};

} // namespace better_std
#endif // BETTER_STD_STD_LCT_HPP
