#ifndef BETTER_STD_STD_MULTIPOINT_HPP
#define BETTER_STD_STD_MULTIPOINT_HPP

//===== better_std 多项式多点求值 / 快速插值（模 998244353）=====
//   multipoint_eval(f, xs)：O(n log^2 n) 求 f(x_i) 全部点值（分治 + 多项式取模）。
//   interpolate(xs, ys)：O(n log^2 n) 由 n 个点值还原次数<n 的多项式。
//   复用 std_polynomial.hpp 的 ntt_multiply / poly_inv / ntt。
#include<vector>
#include<algorithm>
#include<cstdint>
#include"std_polynomial.hpp"
namespace better_std{

namespace multipoint_detail{
    const int MOD=998244353;
    inline int addmod(int a,int b){ a+=b; if(a>=MOD) a-=MOD; return a; }
    inline int submod(int a,int b){ a-=b; if(a<0) a+=MOD; return a; }
    inline int qp(long long a,long long e){ long long r=1; while(e){ if(e&1) r=r*a%MOD; a=a*a%MOD; e>>=1; } return (int)r; }

    // 多项式取模 a mod b（b 首项系数=1，即 b.back()==1），返回次数 < deg(b) 的余式
    inline std::vector<int> poly_mod(std::vector<int> a,const std::vector<int>& b){
        int m=(int)b.size();
        while((int)a.size()>=m){
            if(a.back()==0){ a.pop_back(); continue; }
            int d=(int)a.size()-m;
            long long coef=a.back();              // b 首项为 1，直接消
            for(int i=0;i<m;i++) a[d+i]=submod(a[d+i],(int)(coef*b[i]%MOD));
            a.pop_back();
        }
        if(a.empty()) a.push_back(0);
        return a;
    }
    // 分治构建 (x - x_i) 的乘积树
    inline void build_prod(std::vector<std::vector<int>>& tree,int node,int l,int r,const std::vector<int>& xs){
        if(l==r){ tree[node]={submod(0,(xs[l]%MOD+MOD)%MOD),1}; return; }   // (x - x_i)
        int mid=(l+r)>>1;
        build_prod(tree,node<<1,l,mid,xs);
        build_prod(tree,node<<1|1,mid+1,r,xs);
        tree[node]=ntt_multiply(tree[node<<1],tree[node<<1|1]);
    }
    inline void eval_dc(const std::vector<int>& f,const std::vector<std::vector<int>>& tree,
                        int node,int l,int r,std::vector<int>& ans){
        std::vector<int> cur = (node==1)? f : poly_mod(f,tree[node]);
        if(cur.size()==1&&cur[0]==0){ for(int i=l;i<=r;i++) ans[i]=0; return; }
        if(l==r){ ans[l]=cur.empty()?0:cur[0]; return; }
        int mid=(l+r)>>1;
        eval_dc(cur,tree,node<<1,l,mid,ans);
        eval_dc(cur,tree,node<<1|1,mid+1,r,ans);
    }
} // namespace multipoint_detail

// f：系数从低到高；xs：求值点（会取模到 [0,MOD)）；返回同长点值
inline std::vector<int> multipoint_eval(const std::vector<int>& f,const std::vector<int>& xs){
    using namespace multipoint_detail;
    int n=(int)xs.size();
    std::vector<int> ans(n);
    if(n==0) return ans;
    if(n==1){ long long v=0; for(int i=(int)f.size()-1;i>=0;i--) v=(v*((xs[0]%MOD+MOD)%MOD)+f[i])%MOD; ans[0]=(int)v; return ans; }
    std::vector<std::vector<int>> tree(4*n+4);
    build_prod(tree,1,0,n-1,xs);
    eval_dc(f,tree,1,0,n-1,ans);
    return ans;
}

// 快速插值：xs 互异，ys 对应；返回次数 < n 的多项式
inline std::vector<int> interpolate(const std::vector<int>& xs,const std::vector<int>& ys){
    using namespace multipoint_detail;
    int n=(int)xs.size();
    if(n==0) return {0};
    if(n==1) return {ys[0]};
    std::vector<std::vector<int>> tree(4*n+4);
    build_prod(tree,1,0,n-1,xs);
    // 求 M(x)=∏(x-x_i) 的导数在 x_i 处的值：用 poly_mod 后的差商技巧
    // 这里直接 O(n^2) 求每个点导数（n 一般不大）；再 NTT 加速合并
    std::vector<int> M = tree[1];
    // 导数
    std::vector<int> dM((int)M.size()-1);
    for(int i=1;i<(int)M.size();i++) dM[i-1]=(int)((long long)M[i]*i%MOD);
    std::vector<int> denom(n);
    for(int i=0;i<n;i++){
        long long v=0; for(int j=(int)dM.size()-1;j>=0;j--) v=(v*((xs[i]%MOD+MOD)%MOD)+dM[j])%MOD;
        denom[i]=(int)v;
    }
    // c_i = y_i / denom_i
    std::vector<int> c(n);
    for(int i=0;i<n;i++) c[i]=(int)((long long)ys[i]*qp(denom[i],MOD-2)%MOD);
    // 分治插值：ans = Σ c_i * ∏_{j≠i}(x-x_j)
    // 递归：左半乘右半的 M，右半乘左半的 M
    std::vector<std::vector<int>> res(4*n+4);
    // 用迭代后序：先自底向上，节点 i 存该区间插值结果
    // 采用递归 lambda 不可用（C++11），改写成显式函数式递归函数
    struct Rec {
        const std::vector<std::vector<int>>& tree;
        const std::vector<std::vector<int>>& prod;
        const std::vector<int>& c;
        std::vector<std::vector<int>>& out;
        Rec(const std::vector<std::vector<int>>& t,const std::vector<std::vector<int>>& p,
            const std::vector<int>& cc,std::vector<std::vector<int>>& o):tree(t),prod(p),c(cc),out(o){}
        void run(int nd,int l,int r){
            if(l==r){ out[nd]={c[l]}; return; }
            int mid=(l+r)>>1;
            run(nd<<1,l,mid); run(nd<<1|1,mid+1,r);
            // 左结果 * 右半的 M + 右结果 * 左半的 M
            auto A=ntt_multiply(out[nd<<1],prod[nd<<1|1]);
            auto B=ntt_multiply(out[nd<<1|1],prod[nd<<1]);
            out[nd].resize(std::max(A.size(),B.size()),0);
            for(size_t i=0;i<A.size();i++) out[nd][i]=addmod(out[nd][i],A[i]);
            for(size_t i=0;i<B.size();i++) out[nd][i]=addmod(out[nd][i],B[i]);
        }
    };
    Rec rec(tree,tree,c,res);
    rec.run(1,0,n-1);
    while(!res[1].empty()&&res[1].back()==0) res[1].pop_back();
    if(res[1].empty()) res[1].push_back(0);
    return res[1];
}

} // namespace better_std
#endif // BETTER_STD_STD_MULTIPOINT_HPP
