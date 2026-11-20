#ifndef BETTER_STD_STD_MATRIX_TREE_HPP
#define BETTER_STD_STD_MATRIX_TREE_HPP

//===== better_std 矩阵树定理（生成树计数）=====
//   无向图（可重边）；删去一行一列后取拉普拉斯余子式的行列式。
//   T 需支持四则与 abs（int/ll/High_precision_number 皆可）。
//   提供一般版 spanning_tree_count 与模素数版 spanning_tree_count_mod（避免大整数膨胀）。
#include<vector>
#include<cstdint>
#include"std_matrix.hpp"
namespace better_std{

// 一般版：返回生成树个数（T 为整数型）
template<typename T=long long>
T spanning_tree_count(int n,const std::vector<std::pair<int,int>>& edges){
    if(n<=1) return T(1);
    std::vector<std::vector<T>> L(n,std::vector<T>(n,T(0)));
    for(auto& e:edges){
        int u=e.first, v=e.second;
        L[u][u]+=T(1); L[v][v]+=T(1);
        L[u][v]-=T(1); L[v][u]-=T(1);
    }
    // 删去最后一行一列
    std::vector<std::vector<T>> M(n-1,std::vector<T>(n-1,T(0)));
    for(int i=0;i<n-1;i++) for(int j=0;j<n-1;j++) M[i][j]=L[i][j];
    matrix<T> mat(M);
    return mat.det();
}

// 模素数版：拉普拉斯在 mod 下构造，高斯消元求行列式
inline long long spanning_tree_count_mod(int n,const std::vector<std::pair<int,int>>& edges,long long mod){
    if(n<=1) return 1%mod;
    std::vector<std::vector<long long>> M(n-1,std::vector<long long>(n-1,0));
    for(auto& e:edges){
        int u=e.first, v=e.second;
        if(u<n-1) M[u][u]=(M[u][u]+1)%mod;
        if(v<n-1) M[v][v]=(M[v][v]+1)%mod;
        if(u<n-1&&v<n-1){ M[u][v]=(M[u][v]-1+mod)%mod; M[v][u]=(M[v][u]-1+mod)%mod; }
    }
    long long det=1;
    int m=n-1;
    for(int col=0;col<m;col++){
        int piv=-1;
        for(int r=col;r<m;r++) if(M[r][col]%mod!=0){ piv=r; break; }
        if(piv<0) return 0;
        if(piv!=col){ std::swap(M[piv],M[col]); det=(mod-det)%mod; }
        long long pv=M[col][col]%mod; if(pv<0) pv+=mod;
        det=det*pv%mod;
        long long inv=1;
        { long long a=pv,b=mod-2; while(b){ if(b&1) inv=inv*a%mod; a=a*a%mod; b>>=1; } }
        for(int j=col;j<m;j++) M[col][j]=M[col][j]*inv%mod;
        for(int r=col+1;r<m;r++){
            long long f=M[r][col]%mod; if(f<0) f+=mod;
            if(!f) continue;
            for(int j=col;j<m;j++) M[r][j]=(M[r][j]-f*M[col][j])%mod, M[r][j]<0?M[r][j]+=mod:0;
        }
    }
    return det%mod;
}

} // namespace better_std
#endif // BETTER_STD_STD_MATRIX_TREE_HPP
