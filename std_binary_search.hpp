#pragma once
//===== better_std::std_binary_search —— 二分 / 三分搜索工具 =====
//  first_true(f,lo,hi)  : 最小 x∈[lo,hi] 使 f(x) 为真（f 单调整 False→True）；全假返回 hi+1
//  last_true(f,lo,hi)   : 最大 x∈[lo,hi] 使 f(x) 为真（f 单调整 True→False）；全假返回 lo-1
//  ternary_search_min/max      : 连续单峰函数最小值/最大值（默认 100 次迭代）
//  ternary_search_min/max_discrete : 离散整数单峰最小值/最大值
#include<bits/stdc++.h>
namespace better_std {

// 最小满足条件的 x（f 在 [lo,hi] 上单调：前段假、后段真）
template<typename F>
long long first_true(F f, long long lo, long long hi) {
    long long ans = hi + 1;
    while (lo <= hi) {
        long long mid = lo + (hi - lo) / 2;
        if (f(mid)) { ans = mid; hi = mid - 1; }
        else         lo = mid + 1;
    }
    return ans;
}

// 最大满足条件的 x（f 在 [lo,hi] 上单调：前段真、后段假）
template<typename F>
long long last_true(F f, long long lo, long long hi) {
    long long ans = lo - 1;
    while (lo <= hi) {
        long long mid = lo + (hi - lo) / 2;
        if (f(mid)) { ans = mid; lo = mid + 1; }
        else         hi = mid - 1;
    }
    return ans;
}

// 连续函数最小值（单峰）
template<typename F>
double ternary_search_min(F f, double lo, double hi, int iters = 100) {
    for (int i = 0; i < iters; i++) {
        double m1 = lo + (hi - lo) / 3.0;
        double m2 = hi - (hi - lo) / 3.0;
        if (f(m1) < f(m2)) hi = m2;
        else               lo = m1;
    }
    return (lo + hi) / 2.0;
}

// 连续函数最大值（单峰）
template<typename F>
double ternary_search_max(F f, double lo, double hi, int iters = 100) {
    for (int i = 0; i < iters; i++) {
        double m1 = lo + (hi - lo) / 3.0;
        double m2 = hi - (hi - lo) / 3.0;
        if (f(m1) > f(m2)) hi = m2;
        else               lo = m1;
    }
    return (lo + hi) / 2.0;
}

// 离散整数函数最小值（单峰）
template<typename F>
long long ternary_search_min_discrete(F f, long long lo, long long hi) {
    while (hi - lo > 2) {
        long long m1 = lo + (hi - lo) / 3;
        long long m2 = hi - (hi - lo) / 3;
        if (f(m1) <= f(m2)) hi = m2;
        else                lo = m1;
    }
    long long best = lo;
    for (long long x = lo; x <= hi; x++) if (f(x) < f(best)) best = x;
    return best;
}

// 离散整数函数最大值（单峰）
template<typename F>
long long ternary_search_max_discrete(F f, long long lo, long long hi) {
    while (hi - lo > 2) {
        long long m1 = lo + (hi - lo) / 3;
        long long m2 = hi - (hi - lo) / 3;
        if (f(m1) >= f(m2)) hi = m2;
        else                lo = m1;
    }
    long long best = lo;
    for (long long x = lo; x <= hi; x++) if (f(x) > f(best)) best = x;
    return best;
}

} // namespace better_std
