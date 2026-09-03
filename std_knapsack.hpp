#pragma once
//===== better_std 背包 DP：0/1 / 完全 / 多重 / 子集和 =====
#include<vector>
#include<algorithm>
namespace better_std{

// 0/1 背包：最大价值（容量 W）
inline long long knapsack_01(const std::vector<int>& w, const std::vector<int>& v, int W){
    int n = (int)w.size();
    std::vector<long long> dp(W + 1, 0);
    for(int i = 0; i < n; i++)
        for(int c = W; c >= w[i]; c--)
            dp[c] = std::max(dp[c], dp[c - w[i]] + v[i]);
    return dp[W];
}

// 完全背包：每种无限件
inline long long knapsack_complete(const std::vector<int>& w, const std::vector<int>& v, int W){
    int n = (int)w.size();
    std::vector<long long> dp(W + 1, 0);
    for(int i = 0; i < n; i++)
        for(int c = w[i]; c <= W; c++)
            dp[c] = std::max(dp[c], dp[c - w[i]] + v[i]);
    return dp[W];
}

// 多重背包：cnt[i] 件（二进制拆分转 0/1，等价且高效）
inline long long knapsack_multiple(const std::vector<int>& w,
                                    const std::vector<int>& v,
                                    const std::vector<int>& cnt, int W){
    int n = (int)w.size();
    std::vector<int> ww, vv;
    for(int i = 0; i < n; i++){
        int c = cnt[i];
        for(int k = 1; k <= c; k <<= 1){
            ww.push_back(w[i] * k);
            vv.push_back(v[i] * k);
            c -= k;
        }
        if(c > 0){ ww.push_back(w[i] * c); vv.push_back(v[i] * c); }
    }
    return knapsack_01(ww, vv, W);
}

// 子集和可行性：能否选若干物品重量和恰为 W
inline bool subset_sum(const std::vector<int>& w, int W){
    int n = (int)w.size();
    std::vector<char> dp(W + 1, 0); dp[0] = 1;
    for(int i = 0; i < n; i++)
        for(int c = W; c >= w[i]; c--)
            dp[c] = dp[c] || dp[c - w[i]];
    return dp[W];
}

} // namespace better_std
