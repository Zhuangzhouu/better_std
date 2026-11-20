#ifndef BETTER_STD_COMPAT_HPP
#define BETTER_STD_COMPAT_HPP
//===== better_std ISO C++ / GNU 扩展兼容层 =====
//  判定依据：
//   · __STRICT_ANSI__ : GCC/Clang 在 -std=c++XX（严格 ISO）下定义，-std=gnu++XX 下不定义。
//   · __SIZEOF_INT128__ : 编译器提供 __int128（GCC/Clang 64 位）。
//  因此：-std=c++17 -pedantic-errors 全走纯 ISO 实现；-std=gnu++17 自动启用 GNU 扩展加速。
//  非 GCC/Clang 编译器（MSVC 等）也一律走 ISO 回退。
//  本文件只依赖标准头，无任何扩展类型/内建函数出现在严格 ISO 编译路径中。
#include<cstdint>

#if defined(__SIZEOF_INT128__) && !defined(__STRICT_ANSI__)
#define BETTER_STD_HAS_INT128 1
#else
#define BETTER_STD_HAS_INT128 0
#endif
#if (defined(__GNUC__) || defined(__clang__)) && !defined(__STRICT_ANSI__)
#define BETTER_STD_HAS_GNU_BUILTIN 1
#else
#define BETTER_STD_HAS_GNU_BUILTIN 0
#endif

namespace better_std{
    namespace compat{
        // (a*b) mod m，m>0；先把 a,b 归一到 [0,m)，全程无溢出（含 m 接近 2^63 的情况）
        inline long long mul_mod_64(long long a,long long b,long long m){
            if(m<=1) return 0;
            a%=m; if(a<0) a+=m;
            b%=m; if(b<0) b+=m;
#if BETTER_STD_HAS_INT128
            return (long long)((__int128)a*b%m);
#else
            //纯 ISO：俄罗斯 peasant 加法倍增；add_mod 用条件减法避免 x+y 溢出
            unsigned long long ua=(unsigned long long)a,ub=(unsigned long long)b;
            unsigned long long um=(unsigned long long)m,r=0;
            auto add_mod=[&](unsigned long long x,unsigned long long y)->unsigned long long{
                if(x>=um-y) return x-(um-y);   //x+y>=um → x+y-um
                return x+y;                   //x+y<um，不溢出（um<=2^63-1）
            };
            while(ub){
                if(ub&1ull) r=add_mod(r,ua);
                ua=add_mod(ua,ua);
                ub>>=1;
            }
            return (long long)r;
#endif
        }
        //floor(log2(x))，要求 x>0（对应 31-__builtin_clz(x)）
        inline int ilog2_32(unsigned int x){
#if BETTER_STD_HAS_GNU_BUILTIN
            return 31-__builtin_clz(x);
#else
            int r=0;
            while(x>>=1) r++;
            return r;
#endif
        }
        inline int popcount64(unsigned long long x){
#if BETTER_STD_HAS_GNU_BUILTIN
            return __builtin_popcountll(x);
#else
            int c=0;
            while(x){ x&=x-1; c++; }
            return c;
#endif
        }
    }
}
#endif // BETTER_STD_COMPAT_HPP
