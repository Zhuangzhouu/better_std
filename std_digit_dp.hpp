#pragma once
//===== better_std 数位 DP（通用引擎 + 常用封装） =====
//digit_dp(limit, n_states, init_state, next_state, accept, allow_zero):
//  计数 [0, limit] 中满足条件的整数个数。把 limit 当作十进制串自高位到低位 DP，
//  记忆化 (pos, state, tight, leading)。
//    next_state(state, digit) -> 新状态；只应返回 [0, n_states) 内的值。
//    accept(state)            -> 该完整数是否合法。
//  前导零不消费状态转移（保持 init_state、leading=true），直到遇到首个非零数字。
//  全程前导零（即数值 0）按 accept(init_state) 判定，可用 allow_zero=false 排除 0。
//
//附带常用封装：count_digit_sum / count_without_digit。
#include <string>
#include <vector>
#include <functional>

namespace better_std {

template<typename NextState, typename Accept>
long long digit_dp(long long limit, int n_states, int init_state,
                   NextState next_state, Accept accept, bool allow_zero = true) {
    if (limit < 0) return 0;
    std::string s = std::to_string(limit);
    int L = (int)s.size();
    // memo[pos][state][tight][leading] = -1 表示未计算
    std::vector<std::vector<std::vector<std::vector<long long>>>> memo(
        L, std::vector<std::vector<std::vector<long long>>>(
            n_states, std::vector<std::vector<long long>>(
                2, std::vector<long long>(2, -1))));

    std::function<long long(int,int,bool,bool)> dfs =
        [&](int pos, int state, bool tight, bool leading) -> long long {
            if (pos == L) {
                if (leading) return (allow_zero && accept(init_state)) ? 1 : 0;
                return accept(state) ? 1 : 0;
            }
            long long& res = memo[pos][state][tight][leading];
            if (res != -1) return res;
            int up = tight ? (s[pos] - '0') : 9;
            res = 0;
            for (int d = 0; d <= up; ++d) {
                bool ntight = tight && (d == up);
                if (leading && d == 0) {
                    res += dfs(pos + 1, state, ntight, true);   // 前导零：状态不变
                } else {
                    int ns = next_state(state, d);
                    res += dfs(pos + 1, ns, ntight, false);
                }
            }
            return res;
        };
    return dfs(0, init_state, true, true);
}

// 区间版本：返回 [lo, hi] 内满足条件的个数（lo<=hi）
template<typename NextState, typename Accept>
long long digit_dp_range(long long lo, long long hi, int n_states, int init_state,
                         NextState next_state, Accept accept, bool allow_zero = true) {
    if (lo > hi) return 0;
    long long fhi = digit_dp(hi, n_states, init_state, next_state, accept, allow_zero);
    long long flo = (lo <= 0) ? 0
                              : digit_dp(lo - 1, n_states, init_state, next_state, accept, allow_zero);
    return fhi - flo;
}

// 常用封装：数位和恰为 target 的个数（[0, limit]）
inline long long count_digit_sum(long long limit, int target) {
    if (target < 0) return 0;
    int nst = target + 2;                      // 状态 0..target+1，超界置为 target+1
    auto next_state = [target](int s, int d) {
        int v = s + d; return v > target ? target + 1 : v;
    };
    auto accept = [target](int s) { return s == target; };
    return digit_dp(limit, nst, 0, next_state, accept, true);
}

// 常用封装：不含数字 forbid 的个数（[0, limit]）。forbid==0 时排除数值 0 本身。
inline long long count_without_digit(long long limit, int forbid) {
    int nst = 2;                               // 0=合法, 1=已含禁用数字
    auto next_state = [forbid](int s, int d) {
        if (s == 1) return 1;
        return (d == forbid) ? 1 : 0;
    };
    auto accept = [](int s) { return s == 0; };
    bool allow_zero = (forbid != 0);           // 禁用 0 时不应把数值 0 算作"不含 0"
    return digit_dp(limit, nst, 0, next_state, accept, allow_zero);
}

} // namespace better_std
