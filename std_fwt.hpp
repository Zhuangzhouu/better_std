#ifndef BETTER_STD_STD_FWT_HPP
#define BETTER_STD_STD_FWT_HPP

//===== better_std 快速沃尔什变换 FWT + 快速 zeta 变换 FST =====
//   模素数（默认 998244353）。数组长度须为 2 的幂。
//   FWT：AND / OR / XOR 卷积（O(n log n)）；FST：子集/超集 zeta 变换（O(n log n)）+ 子集卷积（O(n^2 2^n)）。
//   API：fwt_and / fwt_or / fwt_xor(in-place, invert)、and_conv / or_conv / xor_conv、
//        fst_subset / fst_superset(in-place, invert)、subset_conv / superset_conv。
#include<vector>
#include<algorithm>
#include<cstddef>
#include"std_compat.hpp"
namespace better_std{
namespace fwt_detail{
    const int MOD=998244353;
    inline int addmod(int a,int b){ a+=b; if(a>=MOD)a-=MOD; return a; }
    inline int submod(int a,int b){ a-=b; if(a<0)a+=MOD; return a; }
    inline int norm(long long x){ x%=MOD; if(x<0)x+=MOD; return (int)x; }
    inline int popcnt(unsigned x){ return compat::popcount64(x); }
    // 数组长度 2^m 的位数 m（用 compat 的 ISO/GNU 双路径）
    inline int bits_of(int n){ return n<=1?0:compat::ilog2_32((unsigned)n); }
}

//—— FWT-AND：正变换 a[S]=Σ_{T⊇S} b[T]，逆变换反演（自逆，无需缩放）——
inline void fwt_and(std::vector<int>& a,bool invert){
    int n=(int)a.size();
    using namespace fwt_detail;
    for(int len=1;len<n;len<<=1)
        for(int i=0;i<n;i+=len<<1)
            for(int j=0;j<len;j++){
                if(!invert) a[i+j]=addmod(a[i+j],a[i+j+len]);
                else       a[i+j]=submod(a[i+j],a[i+j+len]);
            }
}
//—— FWT-OR：正变换 a[S]=Σ_{T⊆S} b[T] ——
inline void fwt_or(std::vector<int>& a,bool invert){
    int n=(int)a.size();
    using namespace fwt_detail;
    for(int len=1;len<n;len<<=1)
        for(int i=0;i<n;i+=len<<1)
            for(int j=0;j<len;j++){
                if(!invert) a[i+j+len]=addmod(a[i+j+len],a[i+j]);
                else       a[i+j+len]=submod(a[i+j+len],a[i+j]);
            }
}
//—— FWT-XOR：Hadamard 变换，逆变换需乘 1/2 ——
inline void fwt_xor(std::vector<int>& a,bool invert){
    int n=(int)a.size();
    using namespace fwt_detail;
    const int inv2=(MOD+1)/2;
    for(int len=1;len<n;len<<=1)
        for(int i=0;i<n;i+=len<<1)
            for(int j=0;j<len;j++){
                int x=a[i+j],y=a[i+j+len];
                a[i+j]=addmod(x,y);
                a[i+j+len]=submod(x,y);
                if(invert){ a[i+j]=(int)((long long)a[i+j]*inv2%MOD); a[i+j+len]=(int)((long long)a[i+j+len]*inv2%MOD); }
            }
}

inline std::vector<int> and_conv(std::vector<int> a,std::vector<int> b){
    fwt_and(a,false); fwt_and(b,false);
    for(size_t i=0;i<a.size();i++) a[i]=fwt_detail::norm((long long)a[i]*b[i]);
    fwt_and(a,true); return a;
}
inline std::vector<int> or_conv(std::vector<int> a,std::vector<int> b){
    fwt_or(a,false); fwt_or(b,false);
    for(size_t i=0;i<a.size();i++) a[i]=fwt_detail::norm((long long)a[i]*b[i]);
    fwt_or(a,true); return a;
}
inline std::vector<int> xor_conv(std::vector<int> a,std::vector<int> b){
    fwt_xor(a,false); fwt_xor(b,false);
    for(size_t i=0;i<a.size();i++) a[i]=fwt_detail::norm((long long)a[i]*b[i]);
    fwt_xor(a,true); return a;
}

//—— FST：subset zeta 变换，f[S] ← Σ_{T⊆S} a[T]（invert 为 Möbius 反演）——
inline void fst_subset(std::vector<int>& a,bool invert){
    int n=(int)a.size(), m=fwt_detail::bits_of(n);
    using namespace fwt_detail;
    for(int b=0;b<m;b++)
        for(int i=0;i<n;i++) if(i&(1<<b)){
            if(!invert) a[i]=addmod(a[i],a[i^(1<<b)]);
            else       a[i]=submod(a[i],a[i^(1<<b)]);
        }
}
//—— FST：superset zeta 变换，f[S] ← Σ_{T⊇S} a[T] ——
inline void fst_superset(std::vector<int>& a,bool invert){
    int n=(int)a.size(), m=fwt_detail::bits_of(n);
    using namespace fwt_detail;
    for(int b=0;b<m;b++)
        for(int i=0;i<n;i++) if(!(i&(1<<b))){
            if(!invert) a[i]=addmod(a[i],a[i|(1<<b)]);
            else       a[i]=submod(a[i],a[i|(1<<b)]);
        }
}

// 子集卷积：c[S]=Σ_{A∪B=S, A∩B=∅} f[A]·g[B]，n=元素个数（数组长 2^n）。
// 实现：O(3^n) 直接枚举 s 的所有子集 a，b=s^a 与之不交且并集为 s。n<=18 时高效可靠。
inline std::vector<int> subset_conv(const std::vector<int>& f,const std::vector<int>& g,int n){
    int N=1<<n;
    using namespace fwt_detail;
    std::vector<int> res(N,0);
    for(int s=0;s<N;s++){
        long long acc=0;
        for(int a=s;;a=(a-1)&s){
            int b=s^a;
            acc=(acc+(long long)f[a]*g[b])%MOD;
            if(a==0) break;
        }
        res[s]=(int)acc;
    }
    return res;
}

} // namespace better_std
#endif // BETTER_STD_STD_FWT_HPP
