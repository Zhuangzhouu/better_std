#ifndef BETTER_STD_STD_POLYA_HPP
#define BETTER_STD_STD_POLYA_HPP

//===== better_std Burnside / Polya 计数 =====
//   burnside(permutations, k)：k 色给 n 个点染色，在置换群作用下的不同方案数（轨道数）。
//   polya_necklace(n,k)：n 颗珠子 k 色的项链数（旋转群，最经典 Polya）。
//   polya_bracelet(n,k)：n 颗珠子 k 色的手链数（旋转+翻转，二面体群）。
//   结果可能很大，用 long long（溢出由调用方选大整数类型替代）。
#include<vector>
#include<cstdint>
namespace better_std{

// 计算置换的循环个数
inline int permutation_cycles(const std::vector<int>& p){
    int n=(int)p.size(), cyc=0;
    std::vector<char> vis(n,0);
    for(int i=0;i<n;i++) if(!vis[i]){
        cyc++;
        int u=i;
        while(!vis[u]){ vis[u]=1; u=p[u]; }
    }
    return cyc;
}

// Burnside 引理：轨道数 = (1/|G|) Σ_{g∈G} k^{cycles(g)}
inline long long burnside(const std::vector<std::vector<int>>& perms,int k){
    if(perms.empty()) return 0;
    long long sum=0;
    for(auto& p:perms){
        long long t=1, e=permutation_cycles(p);
        for(int i=0;i<e;i++) t*=k;
        sum+=t;
    }
    return sum/(long long)perms.size();
}

namespace polya_detail{
    inline long long ipow(long long a,int e){ long long r=1; while(e){ if(e&1) r*=a; a*=a; e>>=1; } return r; }
}

// 项链（只旋转）：(1/n) Σ_{d|n} φ(d) k^{n/d}
inline long long polya_necklace(int n,int k){
    if(n<=0) return 0;
    long long sum=0;
    for(int d=1;d<=n;d++) if(n%d==0){
        int orig=d, ph=d;                // φ(d)
        for(int p=2;p*p<=orig;p++) if(orig%p==0){
            ph=ph/p*(p-1);
            while(orig%p==0) orig/=p;
        }
        if(orig>1) ph=ph/orig*(orig-1);
        sum += (long long)ph * polya_detail::ipow(k,n/d);
    }
    return sum/n;
}

// 手链（旋转+翻转，二面体群）
inline long long polya_bracelet(int n,int k){
    if(n<=0) return 0;
    long long sum=polya_necklace(n,k)*n;  // 旋转部分 Σ
    // 翻转部分：n 奇 → n 个置换每个 (n+1)/2 循环；n 偶 → n/2 个 n/2 循环 + n/2 个 n/2+1 循环
    if(n&1) sum += (long long)n * polya_detail::ipow(k,(n+1)/2);
    else sum += (long long)(n/2) * (polya_detail::ipow(k,n/2) + polya_detail::ipow(k,n/2+1));
    return sum/(2*n);
}

} // namespace better_std
#endif // BETTER_STD_STD_POLYA_HPP
