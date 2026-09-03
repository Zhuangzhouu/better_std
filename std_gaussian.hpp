#pragma once
//===== better_std 高斯消元：实数 / 模素数 / 异或方程组 =====
#include<vector>
#include<bitset>
#include<cmath>
#include<algorithm>
namespace better_std{

// 实数域：解 A x = b。code: 0 唯一解(sol 有效), 1 无穷解, -1 无解
struct gauss_ret{ int code; std::vector<double> sol; };
inline gauss_ret gauss_solve(std::vector<std::vector<double>> A, std::vector<double> b){
    int n = (int)A.size(), m = (int)A[0].size();
    std::vector<std::vector<double>> M(n, std::vector<double>(m + 1));
    for(int i = 0; i < n; i++){ for(int j = 0; j < m; j++) M[i][j] = A[i][j]; M[i][m] = b[i]; }
    const double eps = 1e-9;
    int rank = 0;
    for(int col = 0; col < m; col++){
        int piv = -1;
        for(int r = rank; r < n; r++) if(std::fabs(M[r][col]) > eps){ piv = r; break; }
        if(piv == -1) continue;
        std::swap(M[rank], M[piv]);
        double d = M[rank][col];
        for(int j = col; j <= m; j++) M[rank][j] /= d;
        for(int r = 0; r < n; r++) if(r != rank && std::fabs(M[r][col]) > eps){
            double f = M[r][col];
            for(int j = col; j <= m; j++) M[r][j] -= f * M[rank][j];
        }
        rank++;
    }
    for(int i = rank; i < n; i++) if(std::fabs(M[i][m]) > eps) return {-1, {}};
    if(rank < m) return {1, {}};
    std::vector<double> sol(m);
    for(int i = 0; i < rank; i++) sol[i] = M[i][m];
    return {0, sol};
}

// 模素数：A x = b (mod mod)。code 同 gauss_ret（用 long long 版）
struct gauss_mod_ret{ int code; std::vector<long long> sol; };
inline gauss_mod_ret gauss_mod(std::vector<std::vector<long long>> A, std::vector<long long> b, long long mod){
    int n = (int)A.size(), m = (int)A[0].size();
    std::vector<std::vector<long long>> M(n, std::vector<long long>(m + 1));
    for(int i = 0; i < n; i++){ for(int j = 0; j < m; j++) M[i][j] = ((A[i][j] % mod) + mod) % mod; M[i][m] = ((b[i] % mod) + mod) % mod; }
    auto modv = [&](long long x){ x %= mod; if(x < 0) x += mod; return x; };
    int rank = 0;
    for(int col = 0; col < m; col++){
        int piv = -1;
        for(int r = rank; r < n; r++) if(modv(M[r][col]) != 0){ piv = r; break; }
        if(piv == -1) continue;
        std::swap(M[rank], M[piv]);
        long long d = modv(M[rank][col]);
        long long inv = modinv(d, mod);
        if(inv == 0) return {-1, {}};
        for(int j = col; j <= m; j++) M[rank][j] = (__int128)modv(M[rank][j]) * inv % mod;
        for(int r = 0; r < n; r++) if(r != rank && modv(M[r][col]) != 0){
            long long f = modv(M[r][col]);
            for(int j = col; j <= m; j++){
                long long term = (__int128)f * M[rank][j] % mod;
                M[r][j] = modv(M[r][j] - term);
            }
        }
        rank++;
    }
    for(int i = rank; i < n; i++) if(M[i][m] != 0) return {-1, {}};
    if(rank < m) return {1, {}};
    std::vector<long long> sol(m);
    for(int i = 0; i < rank; i++) sol[i] = M[i][m];
    return {0, sol};
}

// 异或方程组（bitset 模板）：eq[i] 低位 0..vars-1 为系数，第 vars 位为常数。
// 返回自由元个数（无解返回 -1）；sol 给出一组特解（自由元取 0）
template<size_t N>
int xor_gauss(std::vector<std::bitset<N>>& eq, int vars, std::bitset<N>& sol){
    sol.reset();
    int rows = (int)eq.size();
    int col = 0;
    for(int r = 0; r < rows && col < vars; r++){
        int piv = r;
        while(piv < rows && !eq[piv][col]) piv++;
        if(piv == rows){ col++; r--; continue; }
        std::swap(eq[r], eq[piv]);
        for(int i = 0; i < rows; i++) if(i != r && eq[i][col]) eq[i] ^= eq[r];
        col++;
    }
    int rank = 0;
    for(int i = 0; i < rows; i++){
        bool any = false;
        for(int c = 0; c < vars; c++) if(eq[i][c]){ any = true; break; }
        if(!any){
            if(eq[i][vars]) return -1;   // 0 = 1 矛盾
            continue;
        }
        rank++;
        int lead = -1;
        for(int c = 0; c < vars; c++) if(eq[i][c]){ lead = c; break; }
        sol[lead] = eq[i][vars];
    }
    return vars - rank;
}

} // namespace better_std
