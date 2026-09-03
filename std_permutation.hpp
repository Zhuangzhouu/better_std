#pragma once
//===== better_std 排列工具：全排列 / 康托展开 / 逆序对 =====
#include<vector>
#include<algorithm>
#include<cassert>
namespace better_std{

// 下一个字典序排列（同 std::next_permutation 语义），存在返回 true
template<typename T>
bool next_perm(std::vector<T>& a){
    return std::next_permutation(a.begin(), a.end());
}
// 上一个字典序排列
template<typename T>
bool prev_perm(std::vector<T>& a){
    return std::prev_permutation(a.begin(), a.end());
}

// 枚举 v 的所有排列（已排序去重后按字典序），对每个排列调用 f
template<typename T, typename F>
void for_each_permutation(std::vector<T> v, F f){
    std::sort(v.begin(), v.end());
    do { f(v); } while(std::next_permutation(v.begin(), v.end()));
}

// 康托展开：排列 -> 字典序排名（0-based）。元素互异即可，内部离散化。
// 注意 n>20 时阶乘溢出 long long，请保证 n<=20。
template<typename T>
long long perm_to_rank(const std::vector<T>& a){
    int n = (int)a.size();
    std::vector<T> b = a;
    std::sort(b.begin(), b.end());
    b.erase(std::unique(b.begin(), b.end()), b.end());
    auto id = [&](const T& x){
        return (int)(std::lower_bound(b.begin(), b.end(), x) - b.begin());
    };
    long long fact = 1, rank = 0;
    for(int i = n - 1; i >= 0; i--){
        int cnt = 0;
        for(int j = i + 1; j < n; j++) if(id(a[j]) < id(a[i])) cnt++;
        rank += (long long)cnt * fact;
        fact *= (n - i);
    }
    return rank;
}

// 逆康托展开：排名 k(0-based) -> 第 k 个字典序排列，元素为 0..n-1。
// 要求 0 <= k < n!。
std::vector<int> rank_to_perm(int n, long long k){
    std::vector<long long> fact(n + 1, 1);
    for(int i = 1; i <= n; i++) fact[i] = fact[i-1] * i;
    assert(k >= 0 && k < fact[n]);
    std::vector<int> avail(n);
    for(int i = 0; i < n; i++) avail[i] = i;
    std::vector<int> res;
    for(int i = n; i >= 1; i--){
        long long f = fact[i-1];
        int idx = (int)(k / f);
        k %= f;
        res.push_back(avail[idx]);
        avail.erase(avail.begin() + idx);
    }
    return res;
}

// 逆序对数量（树状数组 O(n log n)，内部离散化）
template<typename T>
long long inversion_count(const std::vector<T>& a){
    int n = (int)a.size();
    std::vector<T> b = a;
    std::sort(b.begin(), b.end());
    b.erase(std::unique(b.begin(), b.end()), b.end());
    auto id = [&](const T& x){
        return (int)(std::lower_bound(b.begin(), b.end(), x) - b.begin()) + 1; // 1-based
    };
    std::vector<long long> bit(b.size() + 1, 0);
    auto add = [&](int p){
        for(; p < (int)bit.size(); p += p & -p) bit[p]++;
    };
    auto sum = [&](int p){
        long long s = 0; for(; p > 0; p -= p & -p) s += bit[p]; return s;
    };
    long long inv = 0;
    for(int i = 0; i < n; i++){
        int idx = id(a[i]);
        inv += (long long)i - sum(idx);   // 已加入且值 > a[i] 的个数
        add(idx);
    }
    return inv;
}

} // namespace better_std
