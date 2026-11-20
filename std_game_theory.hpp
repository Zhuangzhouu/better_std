#ifndef BETTER_STD_STD_GAME_THEORY_HPP
#define BETTER_STD_STD_GAME_THEORY_HPP

//===== better_std 博弈论：Nim / SG 函数 / 常见公平组合游戏 =====
//   nim_xor(piles)：Nim 和（异或和）；为 0 必败，非 0 必胜。
//   nim_win / nim_first_move：判定胜负 / 给出一步必胜走法（取一堆减小使 nim=0）。
//   sg_dag(n,edges)：DAG 上 SG 函数（mex 后继），edges[u] 为 u 可走到的节点。
//   bash_game(n,m,k)：巴什博弈（每次取 1..m，取最后一个胜），先手是否必胜。
//   stairs_nim(a)：阶梯 Nim（奇数阶石堆异或），先手是否必胜。
#include<vector>
#include<numeric>
#include<algorithm>
namespace better_std{

inline long long nim_xor(const std::vector<long long>& piles){
    long long x=0;
    for(long long p:piles) x^=p;
    return x;
}
inline bool nim_win(const std::vector<long long>& piles){ return nim_xor(piles)!=0; }
// 给出 {堆下标, 取走后剩余值}；无必胜走法返回 {-1,-1}
inline std::pair<int,long long> nim_first_move(const std::vector<long long>& piles){
    long long x=nim_xor(piles);
    if(x==0) return {-1,-1};
    for(size_t i=0;i<piles.size();i++){
        if((piles[i]^x)<piles[i]) return {(int)i,piles[i]^x};
    }
    return {-1,-1};
}

// DAG 上 SG 函数：edges[u] = u 的后继节点列表（须拓扑序编号，即后继编号 < u，或先转置）
// 通用做法：若编号非拓扑，按 u 从 0..n-1 算要求 edges[u] 的后继已算好；否则用 DFS 记忆化。
inline std::vector<int> sg_dag(int n,const std::vector<std::vector<int>>& edges){
    std::vector<int> sg(n,0);
    for(int u=0;u<n;u++){
        std::vector<char> seen(n,0);
        for(int v:edges[u]) if(v>=0&&v<n) seen[sg[v]]=1;
        int g=0; while(g<n&&seen[g]) g++;
        sg[u]=g;
    }
    return sg;
}

// 巴什博弈：n 个物品，每次取 1..m，取最后一个胜；先手必胜 iff n%(m+1)!=0
inline bool bash_game(long long n,long long m){ return m>0 && n%(m+1)!=0; }

// 阶梯 Nim：a[0] 为地面（不计），奇数阶（下标 1,3,5...）异或非 0 则先手必胜
inline bool stairs_nim(const std::vector<long long>& a){
    long long x=0;
    for(size_t i=1;i<a.size();i+=2) x^=a[i];
    return x!=0;
}

} // namespace better_std
#endif // BETTER_STD_STD_GAME_THEORY_HPP
