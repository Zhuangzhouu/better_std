#pragma once
//===== better_std 经典序列 DP（LIS / LCS）=====
#include<vector>
#include<algorithm>
namespace better_std{

// 最长上升子序列长度（严格上升），O(n log n)
template<typename T>
int lis(const std::vector<T>& a){
    std::vector<T> tails;
    for(auto x : a){
        auto it = std::lower_bound(tails.begin(), tails.end(), x);
        if(it == tails.end()) tails.push_back(x);
        else *it = x;
    }
    return (int)tails.size();
}

// 最长上升子序列本身（取最早出现方案），O(n log n)
template<typename T>
std::vector<T> lis_sequence(const std::vector<T>& a){
    int n = (int)a.size();
    if(n == 0) return {};
    std::vector<T> tails;
    std::vector<int> tail_idx;   // 每层 tail 对应的原下标
    std::vector<int> prev(n, -1);
    for(int i = 0; i < n; i++){
        auto it = std::lower_bound(tails.begin(), tails.end(), a[i]);
        int pos = (int)(it - tails.begin());
        if(it == tails.end()){
            tails.push_back(a[i]);
            tail_idx.push_back(i);
        } else {
            tails[pos] = a[i];
            tail_idx[pos] = i;
        }
        if(pos > 0) prev[i] = tail_idx[pos - 1];
    }
    int len = (int)tails.size();
    std::vector<T> res(len);
    int k = len - 1, cur = tail_idx[k];
    while(k >= 0){
        res[k] = a[cur];
        cur = prev[cur];
        k--;
    }
    return res;
}

// 最长公共子序列长度，O(n m)
template<typename T>
int lcs(const std::vector<T>& a, const std::vector<T>& b){
    int n = (int)a.size(), m = (int)b.size();
    std::vector<std::vector<int>> dp(n+1, std::vector<int>(m+1, 0));
    for(int i = 1; i <= n; i++)
        for(int j = 1; j <= m; j++)
            dp[i][j] = (a[i-1] == b[j-1]) ? dp[i-1][j-1] + 1
                                          : std::max(dp[i-1][j], dp[i][j-1]);
    return dp[n][m];
}

// 最长公共子序列本身（回溯），O(n m)
template<typename T>
std::vector<T> lcs_sequence(const std::vector<T>& a, const std::vector<T>& b){
    int n = (int)a.size(), m = (int)b.size();
    std::vector<std::vector<int>> dp(n+1, std::vector<int>(m+1, 0));
    for(int i = 1; i <= n; i++)
        for(int j = 1; j <= m; j++)
            dp[i][j] = (a[i-1] == b[j-1]) ? dp[i-1][j-1] + 1
                                          : std::max(dp[i-1][j], dp[i][j-1]);
    std::vector<T> res;
    int i = n, j = m;
    while(i > 0 && j > 0){
        if(a[i-1] == b[j-1]){ res.push_back(a[i-1]); i--; j--; }
        else if(dp[i-1][j] >= dp[i][j-1]) i--;
        else j--;
    }
    std::reverse(res.begin(), res.end());
    return res;
}

} // namespace better_std
