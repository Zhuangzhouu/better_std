#ifndef BETTER_STD_STD_LINEAR_RECURRENCE_HPP
#define BETTER_STD_STD_LINEAR_RECURRENCE_HPP

//===== better_std 线性递推：Berlekamp-Massey + Kitamasa =====
//   berlekamp_massey(s)：由数列前若干项求最短线性递推式 rec（满足 a_n = Σ rec[i-1]*a_{n-i}）。
//   linear_rec_nth(init,rec,n)：已知初值 init 与递推 rec，求第 n 项（0-based），O(k^2 log n)。
//   模素数（默认 998244353）。
#include<vector>
#include<cstddef>
namespace better_std{
namespace lr_detail{
    const int MOD=998244353;
    inline int addmod(int a,int b){ a+=b; if(a>=MOD)a-=MOD; return a; }
    inline int submod(int a,int b){ a-=b; if(a<0)a+=MOD; return a; }
    inline int norm(long long x){ x%=MOD; if(x<0)x+=MOD; return (int)x; }
    inline int modpow(int a,long long e){ long long r=1; while(e){ if(e&1)r=r*a%MOD; a=(long long)a*a%MOD; e>>=1; } return (int)r; }
}

// BM 算法：返回最短线性递推系数 c[0..k-1]，使 s[n]=Σ_{i=1..k} c[i] s[n-1-i]（c[0] 对应 s[n-1]）
inline std::vector<int> berlekamp_massey(const std::vector<int>& s){
    using namespace lr_detail;
    // C[i] 满足 s[n] + Σ_{i>=1} C[i]*s[n-i] = 0；最终 rec[i] = -C[i+1]
    std::vector<int> C(1,1), B(1,1);
    int L=0, m=1, b=1;
    for(int n=0;n<(int)s.size();n++){
        int d=s[n];
        for(int i=1;i<=(int)C.size()-1 && i<=n;i++) d=addmod(d,norm((long long)C[i]*s[n-i]));
        if(d==0){ m++; continue; }
        std::vector<int> T=C;
        int coef=norm((long long)d*modpow(b,MOD-2));
        if((int)C.size() < (int)B.size()+m) C.resize(B.size()+m,0);
        for(size_t i=0;i<B.size();i++) C[i+m]=submod(C[i+m],norm((long long)coef*B[i]));
        if(2*L<=n){
            L=n+1-L; B=T; b=d; m=1;
        } else m++;
    }
    std::vector<int> rec(L);
    for(int i=1;i<=L && i<(int)C.size();i++) rec[i-1]=submod(0,C[i]);
    while(!rec.empty()&&rec.back()==0) rec.pop_back();
    return rec;
}

namespace lr_detail{
    // 多项式乘法（截断到 limit）
    inline std::vector<int> polymul(const std::vector<int>& a,const std::vector<int>& b,int limit){
        std::vector<int> r(std::min((int)a.size()+(int)b.size()-1,limit),0);
        for(size_t i=0;i<a.size();i++) if(a[i])
            for(size_t j=0;j<b.size() && i+j<r.size();j++) if(b[j])
                r[i+j]=addmod(r[i+j],norm((long long)a[i]*b[j]));
        return r;
    }
    // 特征多项式 P(x) = x^k - c[0] x^{k-1} - ... - c[k-1]，升幂系数 P[k]=1, P[i]=-c[k-1-i]
    inline std::vector<int> char_poly(const std::vector<int>& c){
        int k=(int)c.size();
        std::vector<int> p(k+1,0);
        p[k]=1;
        for(int i=0;i<k;i++) p[i]=submod(0,c[k-1-i]);
        return p;
    }
    // x^n mod P(x)：返回次数 < k 的多项式（P 为升幂，首一）
    inline std::vector<int> x_pow_mod(long long n,const std::vector<int>& P){
        int k=(int)P.size()-1;
        if(k<=0) return {0};
        auto reduce=[&](std::vector<int>& v){
            while((int)v.size()>k){
                int d=(int)v.size()-1;              // 最高次幂
                int coef=v.back();
                v.pop_back();                       // 现在 v 索引 0..d-1
                if(coef==0) continue;
                // x^d ≡ -Σ_{i=0}^{k-1} P[i] x^{d-k+i}
                for(int i=0;i<k;i++) v[d-k+i]=submod(v[d-k+i],norm((long long)coef*P[i]));
                while(!v.empty()&&v.back()==0) v.pop_back();
            }
            if(v.empty()) v.push_back(0);
        };
        std::vector<int> res(1,1);                 // 常数 1
        std::vector<int> base={0,1};               // x
        reduce(base);                              // x mod P（k=1 时化为常数）
        int lim=2*k;                               // 乘法保留到 2k-1 次，供 reduce
        while(n){
            if(n&1){ res=polymul(res,base,lim); reduce(res); }
            base=polymul(base,base,lim); reduce(base);
            n>>=1;
        }
        res.resize(k,0);
        return res;
    }
}

// 由初值 init[0..k-1] 与递推 rec（长度 k）求第 n 项（0-based）
inline int linear_rec_nth(const std::vector<int>& init,const std::vector<int>& rec,long long n){
    using namespace lr_detail;
    int k=(int)rec.size();
    if(k==0) return 0;
    if(n<(long long)init.size()) return init[(size_t)n];
    std::vector<int> P=char_poly(rec);
    std::vector<int> coef=x_pow_mod(n,P);          // x^n ≡ Σ coef[i] x^i (mod P)
    long long ans=0;
    for(int i=0;i<k;i++) ans=(ans+(long long)coef[i]*init[i])%MOD;
    return norm(ans);
}

// 便捷：由序列前 m 项训练出递推，再预测第 n 项
inline int linear_rec_predict(const std::vector<int>& s,long long n){
    std::vector<int> rec=berlekamp_massey(s);
    return linear_rec_nth(s,rec,n);
}

} // namespace better_std
#endif // BETTER_STD_STD_LINEAR_RECURRENCE_HPP
