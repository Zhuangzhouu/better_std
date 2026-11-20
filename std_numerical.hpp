#ifndef BETTER_STD_STD_NUMERICAL_HPP
#define BETTER_STD_STD_NUMERICAL_HPP

//===== better_std 数值计算：Simpson 积分 / 求根 / 拉格朗日插值 =====
//   simpson / adaptive_simpson：定积分（自适应 Simpson，double）。
//   bisect_root / newton_root：求方程根（double，回调 f / f'）。
//   lagrange_interpolate(real)：实数拉格朗日插值求 x0 处值；lagrange_interpolate_mod：模素数整点插值。
#include<vector>
#include<cmath>
#include<cstddef>
namespace better_std{

//—— 辛普森公式（一次）——
template<typename F>
inline double simpson(F f,double l,double r){
    double mid=(l+r)/2;
    return (r-l)/6.0*(f(l)+4*f(mid)+f(r));
}
//—— 自适应辛普森：递归细分至误差 < eps ——
template<typename F>
inline double adaptive_simpson_(F f,double l,double r,double whole,double eps,int depth){
    double mid=(l+r)/2;
    double sl=simpson(f,l,mid), sr=simpson(f,mid,r);
    double sum=sl+sr;
    if(depth<=0 || std::fabs(sum-whole)<15*eps) return sum+(sum-whole)/15.0;   // Richardson 外推
    return adaptive_simpson_(f,l,mid,sl,eps/2,depth-1)+adaptive_simpson_(f,mid,r,sr,eps/2,depth-1);
}
template<typename F>
inline double adaptive_simpson(F f,double l,double r,double eps=1e-9,int maxdepth=20){
    return adaptive_simpson_(f,l,r,simpson(f,l,r),eps,maxdepth);
}

//—— 二分求根：f(l)*f(r)<0，返回根（迭代 100 次或精度达标）——
template<typename F>
inline double bisect_root(F f,double l,double r,double eps=1e-12){
    double fl=f(l);
    for(int it=0;it<200 && r-l>eps;it++){
        double mid=(l+r)/2, fm=f(mid);
        if((fm>0)==(fl>0)){ l=mid; fl=fm; } else r=mid;
    }
    return (l+r)/2;
}
//—— 牛顿迭代求根：f, f'，初值 x0 ——
template<typename F,typename DF>
inline double newton_root(F f,DF df,double x0,double eps=1e-12,int maxit=200){
    double x=x0;
    for(int it=0;it<maxit;it++){
        double fx=f(x);
        if(std::fabs(fx)<eps) return x;
        double d=df(x);
        if(std::fabs(d)<1e-18) break;
        double nx=x-fx/d;
        if(std::fabs(nx-x)<eps) return nx;
        x=nx;
    }
    return x;
}

//—— 实数拉格朗日插值：给定 n 个点 (xs[i], ys[i])，求 x0 处值 ——
inline double lagrange_interpolate(const std::vector<double>& xs,const std::vector<double>& ys,double x0){
    int n=(int)xs.size();
    double ans=0;
    for(int i=0;i<n;i++){
        double term=ys[i];
        for(int j=0;j<n;j++) if(j!=i) term*=(x0-xs[j])/(xs[i]-xs[j]);
        ans+=term;
    }
    return ans;
}

namespace numerical_detail{
    const int MOD=998244353;
    inline int addmod(int a,int b){ a+=b; if(a>=MOD)a-=MOD; return a; }
    inline int submod(int a,int b){ a-=b; if(a<0)a+=MOD; return a; }
    inline int norm(long long x){ x%=MOD; if(x<0)x+=MOD; return (int)x; }
    inline int modpow(int a,long long e){ long long r=1; while(e){ if(e&1)r=r*a%MOD; a=(long long)a*a%MOD; e>>=1; } return (int)r; }
}
//—— 模素数拉格朗日插值（xs 为 [0,n-1] 连续整数，求 x0 处值）——
inline int lagrange_interpolate_mod(const std::vector<int>& ys,long long x0){
    using namespace numerical_detail;
    int n=(int)ys.size();
    if(n==0) return 0;
    x0%=MOD; if(x0<0)x0+=MOD;
    if(x0<n) return ys[(size_t)x0];
    // 前缀积 / 后缀积
    std::vector<int> pre(n+1,1), suf(n+1,1);
    for(int i=0;i<n;i++) pre[i+1]=(int)((long long)pre[i]*((x0-i+MOD)%MOD)%MOD);
    for(int i=n-1;i>=0;i--) suf[i]=(int)((long long)suf[i+1]*((x0-i+MOD)%MOD)%MOD);
    std::vector<int> fac(n+1,1);
    for(int i=1;i<=n;i++) fac[i]=(int)((long long)fac[i-1]*i%MOD);
    long long ans=0;
    for(int i=0;i<n;i++){
        // ∏_{j≠i}(x0-j) = pre[i]*suf[i+1]
        long long num=(long long)pre[i]*suf[i+1]%MOD;
        // 分母 = i! * (-1)^{n-1-i} * (n-1-i)!
        long long den=(long long)fac[i]*fac[n-1-i]%MOD;
        if((n-1-i)&1) den=(MOD-den)%MOD;
        ans=(ans+ys[i]*num%MOD*modpow((int)den,MOD-2))%MOD;
    }
    return norm(ans);
}

} // namespace better_std
#endif // BETTER_STD_STD_NUMERICAL_HPP
