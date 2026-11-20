#ifndef BETTER_STD_STD_PRIME_FACTOR_HPP
#define BETTER_STD_STD_PRIME_FACTOR_HPP

//===== better_std 大整数质因数分解：Miller-Rabin 素性 + Pollard-Rho =====
//   支持 64 位无符号整数（含接近 2^64 的值），用 compat::mul_mod_64 防模乘溢出。
//   API：is_prime64 / pollard_rho / factor_u64 / prime_factors(成对数) / divisor_count / divisor_sum。
#include<cstdint>
#include<vector>
#include<map>
#include<algorithm>
#include<utility>
#include"std_compat.hpp"
namespace better_std{

namespace prime_factor_detail{
    using u64 = unsigned long long;
    using i64 = long long;
    inline u64 gcd_u64(u64 a,u64 b){ while(b){ u64 t=a%b; a=b; b=t; } return a; }
    // [0,m) 上的 (a*b) mod m：m<=2^63-1 走 compat；更大时加法倍增（add_mod 用条件减法避 u64 溢出）
    inline u64 mul_mod(u64 a,u64 b,u64 m){
        if(m==0) return 0;
        if(m <= (u64)LLONG_MAX) return (u64)compat::mul_mod_64((i64)a,(i64)b,(i64)m);
        a%=m; b%=m;
        auto add_mod=[&](u64 x,u64 y)->u64{   // 前提 x,y<m（故 m-y 安全）
            if(x >= m-y) return x-(m-y);     // x+y>=m → x+y-m
            return x+y;                      // x+y<m<2^64，不溢出
        };
        u64 r=0;
        while(b){ if(b&1) r=add_mod(r,a); a=add_mod(a,a); b>>=1; }
        return r;
    }
    inline u64 pow_mod(u64 a,u64 e,u64 m){ u64 r=1%m; while(e){ if(e&1) r=mul_mod(r,a,m); a=mul_mod(a,a,m); e>>=1; } return r; }
    // Miller-Rabin：对 64 位用固定 7 个底数（确定性）
    inline bool miller_rabin(u64 n){
        if(n<2) return false;
        static const u64 small[]={2,3,5,7,11,13,17,19,23,29,31,37};
        for(u64 p:small){ if(n%p==0) return n==p; }
        u64 d=n-1; int s=0; while((d&1)==0){ d>>=1; s++; }
        static const u64 bases[]={2ULL,325ULL,9375ULL,28178ULL,450775ULL,9780504ULL,1795265022ULL};
        for(u64 a:bases){
            if(a%n==0) continue;
            u64 x=pow_mod(a%n,d,n);
            if(x==1||x==n-1) continue;
            bool comp=true;
            for(int r=1;r<s;r++){ x=mul_mod(x,x,n); if(x==n-1){ comp=false; break; } }
            if(comp) return false;
        }
        return true;
    }
} // namespace prime_factor_detail

// Miller-Rabin 素性测试（64 位确定性）
inline bool is_prime64(unsigned long long n){ return prime_factor_detail::miller_rabin(n); }

namespace prime_factor_detail{
    inline u64 f_rnd(u64 x,u64 c,u64 mod){ return (mul_mod(x,x,mod)+c)%mod; }
    // Pollard-Rho（Brent 批次优化）
    inline u64 pollard(u64 n){
        using namespace prime_factor_detail;
        if((n&1)==0) return 2;
        if(n%3==0) return 3;
        static u64 seed=88172645463325252ull;
        for(;;){
            seed^=seed<<13; seed^=seed>>7; seed^=seed<<17;
            u64 c=seed%(n-1)+1, x=seed%n, y=x, d=1;
            for(int lim=1; d==1; lim<<=1){
                x=y;
                for(u64 i=0;i<(u64)lim;i++) y=f_rnd(y,c,n);
                u64 q=1;
                for(u64 k=0;k<(u64)lim && d==1; k+=128){
                    u64 qq=q;
                    for(u64 i=0;i<128 && k+i<(u64)lim;i++){ y=f_rnd(y,c,n); qq=mul_mod(qq, (x>y?x-y:y-x), n); }
                    q=qq;
                    d=gcd_u64(q,n);
                }
                if(d==n){ do{ y=f_rnd(y,c,n); d=gcd_u64(x>y?x-y:y-x,n); }while(d==1); if(d==n) break; }
            }
            if(d>1 && d<n) return d;
        }
    }
    inline void factor_rec(u64 n,std::vector<u64>& out){
        if(n==1) return;
        if(miller_rabin(n)){ out.push_back(n); return; }
        u64 d=pollard(n);
        if(d==n || d==0){ factor_rec(n,out); return; }  // 极端退化，递归重试（换随机种子）
        factor_rec(d,out); factor_rec(n/d,out);
    }
} // namespace prime_factor_detail

// 返回 n 的所有质因数（含重复，升序）
inline std::vector<unsigned long long> factor_u64(unsigned long long n){
    std::vector<unsigned long long> out;
    if(n<2) return out;
    prime_factor_detail::factor_rec(n,out);
    std::sort(out.begin(),out.end());
    return out;
}

// 返回质因数 → 指数的映射
inline std::map<unsigned long long,int> prime_factors(unsigned long long n){
    std::map<unsigned long long,int> mp;
    for(unsigned long long p:factor_u64(n)) mp[p]++;
    return mp;
}

// 约数个数 d(n)
inline unsigned long long divisor_count(unsigned long long n){
    unsigned long long ans=1;
    for(auto& kv:prime_factors(n)) ans*= (unsigned long long)(kv.second+1);
    return ans;
}

// 约数和 σ(n)
inline unsigned long long divisor_sum(unsigned long long n){
    if(n==0) return 0;
    unsigned long long ans=1;
    auto mp=prime_factors(n);
    for(auto& kv:mp){
        unsigned long long p=kv.first; int e=kv.second;
        unsigned long long term=1, pk=1;
        for(int i=0;i<e;i++){ pk*=p; term+=pk; }
        ans*=term;
    }
    return ans;
}

} // namespace better_std
#endif // BETTER_STD_STD_PRIME_FACTOR_HPP
