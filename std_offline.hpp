#ifndef BETTER_STD_STD_OFFLINE_HPP
#define BETTER_STD_STD_OFFLINE_HPP

//===== better_std 离线技巧：整体二分 + 线段树分治（动态二分图）=====
//   · overall_binary_range_kth：整体二分求静态区间第 k 小（主席树替代，O((n+q)logV)）。
//   · segment_tree_divide_bipartite：线段树分治 + 可回滚带权并查集，判定每个时刻动态图是否二分。
#include<vector>
#include<algorithm>
#include<cstddef>
#include<functional>
namespace better_std{

namespace offline_detail{
    // 树状数组（单点加 / 前缀和），支持回滚
    struct bit{
        int n; std::vector<int> t;
        explicit bit(int n_=0):n(n_),t(n_+1,0){}
        void reset(int n_){ n=n_; t.assign(n+1,0); }
        void add(int i,int v){ for(;i<=n;i+=i&-i) t[i]+=v; }
        int sum(int i) const { int r=0; for(;i>0;i-=i&-i) r+=t[i]; return r; }
        int range(int l,int r) const { return sum(r)-sum(l-1); }
    };
}

struct ob_query{ int l,r,k,id; };

// 静态区间第 k 小（查询 ob_query.l/r 为 0-based 闭区间，k>=1）；值域 [vlo,vhi]
inline std::vector<int> overall_binary_range_kth(const std::vector<int>& a,
        const std::vector<ob_query>& qs,int vlo,int vhi){
    int n=(int)a.size();
    std::vector<int> ans(qs.size(),vhi+1);
    if(qs.empty()) return ans;
    offline_detail::bit bit(n);
    std::vector<ob_query> cur(qs), tmp(qs.size());
    // 查询区间转成 1-based 存回 cur
    for(size_t i=0;i<cur.size();i++){ cur[i].l++; cur[i].r++; }
    std::function<void(int,int,int,int)> solve = [&](int vl,int vr,int L,int R){
        if(L>=R) return;
        if(vl==vr){ for(int i=L;i<R;i++) ans[cur[i].id]=vl; return; }
        int mid=(vl+vr)>>1;
        std::vector<int> added;
        for(int i=0;i<n;i++) if(a[i]>=vl && a[i]<=mid){ bit.add(i+1,1); added.push_back(i+1); }
        std::vector<ob_query> left, right;
        for(int i=L;i<R;i++){
            int cnt=bit.range(cur[i].l,cur[i].r);
            if(cnt>=cur[i].k) left.push_back(cur[i]);
            else { ob_query q=cur[i]; q.k-=cnt; right.push_back(q); }
        }
        for(int p:added) bit.add(p,-1);
        for(size_t i=0;i<left.size();i++) cur[L+i]=left[i];
        for(size_t i=0;i<right.size();i++) cur[L+left.size()+i]=right[i];
        int leftCnt=(int)left.size();
        solve(vl,mid,L,L+leftCnt);
        solve(mid+1,vr,L+leftCnt,R);
    };
    solve(vlo,vhi,0,(int)cur.size());
    return ans;
}

namespace offline_detail{
    // 可回滚带奇偶并查集：parity[x]=x 到父节点颜色关系（0 同 1 异）
    struct parity_rollback_dsu{
        std::vector<int> par, rnk, parity;
        std::vector<std::pair<int,int>> hist;   // (child, parent)
        int conflicts;
        explicit parity_rollback_dsu(int n=0){ init(n); }
        void init(int n){ par.resize(n); rnk.assign(n,0); parity.assign(n,0); conflicts=0; hist.clear(); for(int i=0;i<n;i++)par[i]=i; }
        // 返回 {root, 到根的奇偶}
        std::pair<int,int> find_(int x) const {
            int p=x, c=0;
            while(par[p]!=p){ c^=parity[p]; p=par[p]; }
            return {p,c};
        }
        bool unite(int a,int b,bool want_diff){
            auto pa=find_(a), pb=find_(b);
            int xa=pa.first, ca=pa.second;
            int xb=pb.first, cb=pb.second;
            if(xa==xb){ if( ((ca^cb)?1:0) != (want_diff?1:0) ){ conflicts++; return false; } return true; }
            if(rnk[xa]<rnk[xb]){ std::swap(xa,xb); std::swap(ca,cb); }
            par[xb]=xa; parity[xb]=ca^cb^(want_diff?1:0);
            hist.push_back({xb,xa});
            if(rnk[xa]==rnk[xb]) rnk[xa]++;
            return true;
        }
        int checkpoint() const { return (int)hist.size(); }
        int conflict_snapshot() const { return conflicts; }
        void rollback(int cp,int conf){
            while((int)hist.size()>cp){ int c=hist.back().first; hist.pop_back(); par[c]=c; parity[c]=0; }
            conflicts=conf;
        }
    };
}

struct edge_interval{ int u,v,l,r; };   // 边 (u,v) 在时间 [l,r] 存在（0-based 时间）

// 线段树分治：返回长度 T 的数组，answer[t]=该时刻图是否二分
inline std::vector<char> segment_tree_divide_bipartite(int n,int T,const std::vector<edge_interval>& edges){
    std::vector<char> ans(T,1);
    if(T<=0) return ans;
    // 线段树节点存边索引
    int size=4*T;
    std::vector<std::vector<int>> tree(size);
    // 插入区间
    struct Inserter{
        std::vector<std::vector<int>>& tree;
        Inserter(std::vector<std::vector<int>>& t):tree(t){}
        void add(int idx,int l,int r,int ql,int qr,int eid){
            if(ql<=l&&r<=qr){ tree[idx].push_back(eid); return; }
            int mid=(l+r)/2;
            if(ql<=mid) add(idx*2,l,mid,ql,qr,eid);
            if(qr>mid) add(idx*2+1,mid+1,r,ql,qr,eid);
        }
    } ins(tree);
    for(size_t i=0;i<edges.size();i++){
        const auto& e=edges[i];
        if(e.l>e.r||e.r<0||e.l>=T) continue;
        int l=std::max(0,e.l),r=std::min(T-1,e.r);
        ins.add(1,0,T-1,l,r,(int)i);
    }
    offline_detail::parity_rollback_dsu dsu(n);
    // DFS
    struct Dfs{
        const std::vector<std::vector<int>>& tree;
        const std::vector<edge_interval>& edges;
        offline_detail::parity_rollback_dsu& dsu;
        std::vector<char>& ans;
        Dfs(const std::vector<std::vector<int>>& t,const std::vector<edge_interval>& e,
            offline_detail::parity_rollback_dsu& d,std::vector<char>& a):tree(t),edges(e),dsu(d),ans(a){}
        void run(int idx,int l,int r){
            int cp=dsu.checkpoint(), cf=dsu.conflict_snapshot();
            for(int eid:tree[idx]) dsu.unite(edges[eid].u,edges[eid].v,true);
            if(l==r){ ans[l]=(dsu.conflicts==0); }
            else {
                int mid=(l+r)/2;
                run(idx*2,l,mid);
                run(idx*2+1,mid+1,r);
            }
            dsu.rollback(cp,cf);
        }
    } dfs(tree,edges,dsu,ans);
    dfs.run(1,0,T-1);
    return ans;
}

} // namespace better_std
#endif // BETTER_STD_STD_OFFLINE_HPP
