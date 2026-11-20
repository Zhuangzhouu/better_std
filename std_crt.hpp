#ifndef BETTER_STD_STD_CRT_HPP
#define BETTER_STD_STD_CRT_HPP

//===== better_std 中国剩余定理 CRT / 线性同余 / 扩展欧拉定理 =====
//   crt(r,m)：合并同余方程组 x≡r[i] (mod m[i])；互素用 CRT，非互素用 exgcd 逐个合并。
//   linear_congruence(a,b,m)：解 ax≡b (mod m)，返回 {是否有解, 最小非负解 x, 周期 m/g}。
//   crt_general：同上，返回最小非负解与总模数 lcm；无解返回 {false,0,0}。
//   全程用 compat::mul_mod_64 防模乘溢出（m 可到 1e18）。
#include<vector>
#include<utility>
#include<cstdint>
#include"std_compat.hpp"
namespace better_std{
namespace crt_detail{
    using i64=long long;
    inline i64 exgcd(i64 a,i64 b,i64& x,i64& y){
        if(!b){ x=1; y=0; return a; }
        i64 x1,y1; i64 g=crt_detail::exgcd(b,a%b,x1,y1);
        x=y1; y=x1-(a/b)*y1;
        return g;
    }
    inline i64 mod_norm(i64 x,i64 m){ x%=m; if(x<0)x+=m; return x; }
}

// 解 ax≡b (mod m)。返回 {有解?, 最小非负解, 周期 m/g}；无解时解=0、周期=0。
inline std::pair<bool,std::pair<long long,long long>> linear_congruence(long long a,long long b,long long m){
    using namespace crt_detail;
    if(m<=0) return {false,{0,0}};
    i64 xx,yy;
    i64 g=crt_detail::exgcd(a,m,xx,yy);
    if(g<0) g=-g;
    if(b%g!=0) return {false,{0,0}};
    i64 m2=m/g;
    i64 t=crt_detail::mod_norm(b/g,m2);
    i64 inv; i64 dummy;
    crt_detail::exgcd(crt_detail::mod_norm(a/g,m2),m2,inv,dummy);
    i64 x0=(i64)compat::mul_mod_64(t,crt_detail::mod_norm(inv,m2),m2);
    return {true,{x0,m2}};
}

namespace crt_detail{
    // 合并 x≡a1(mod m1) 与 x≡a2(mod m2)，返回 {有解, x, lcm}
    inline std::pair<bool,std::pair<i64,i64>> merge(i64 a1,i64 m1,i64 a2,i64 m2){
        i64 x,y; i64 g=crt_detail::exgcd(m1,m2,x,y);
        i64 diff=a2-a1;
        if(diff%g!=0) return {false,{0,0}};
        i64 lcm=m1/g*m2;
        // x = a1 + m1 * t，其中 t ≡ (diff/g) * inv(m1/g, m2/g) (mod m2/g)
        i64 m2g=m2/g;
        i64 inv; i64 dummy;
        crt_detail::exgcd(mod_norm(m1/g,m2g),m2g,inv,dummy);
        i64 t=mod_norm((i64)compat::mul_mod_64((i64)(diff/g),(i64)mod_norm(inv,m2g),m2g),m2g);
        i64 res=mod_norm(a1+(i64)compat::mul_mod_64((i64)m1,(i64)t,lcm),lcm);
        return {true,{res,lcm}};
    }
}

// 通用 CRT：返回 {有解?, 最小非负解 x, 总模数 lcm}；无解返回 {false,0,0}
inline std::pair<bool,std::pair<long long,long long>> crt_general(const std::vector<long long>& r,const std::vector<long long>& m){
    if(r.size()!=m.size()||r.empty()) return {false,{0,0}};
    long long x=crt_detail::mod_norm(r[0],m[0]), mod=m[0];
    for(size_t i=1;i<m.size();i++){
        auto res=crt_detail::merge(x,mod,crt_detail::mod_norm(r[i],m[i]),m[i]);
        if(!res.first) return {false,{0,0}};
        x=res.second.first; mod=res.second.second;
    }
    return {true,{x,mod}};
}

// 互素模数 CRT（要求两两互素，否则结果不可靠；非互素请用 crt_general）
inline long long crt(const std::vector<long long>& r,const std::vector<long long>& m){
    auto res=crt_general(r,m);
    return res.first?res.second.first:-1;
}

} // namespace better_std
#endif // BETTER_STD_STD_CRT_HPP
