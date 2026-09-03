#pragma once
//===== better_std::mo_algorithm —— 莫队算法（离线区间查询） =====
//对数组 a[0..n-1] 的若干区间查询 [l, r]，按"块排序"后以 O((n+q)√n) 的均摊
//复杂度顺序处理：每次通过 add(idx)/remove(idx) 维护可差分的数据结构，
//并在移动到某个查询时调用 answer(qid) 记录结果。
//
//  回调约定（可调用对象 / lambda）：
//    add(int idx)      —— 把位置 idx 纳入当前窗口
//    remove(int idx)   —— 把位置 idx 移出当前窗口
//    answer(int qid)   —— 当前窗口恰好覆盖第 qid 个查询，记录答案
//
//  查询下标为闭区间 [l, r]，均 0-based。
#include <vector>
#include <algorithm>

namespace better_std {

template<typename Add, typename Remove, typename Answer>
void mo_algorithm(int n, const std::vector<std::pair<int,int>>& queries,
                  Add add, Remove remove, Answer answer) {
    const int q = (int)queries.size();
    if (q == 0) return;

    // 块大小取 √n，奇偶块交错排序以减小指针来回幅度
    int block = 1;
    while ((long long)block * block < n) ++block;

    std::vector<int> ord(q);
    for (int i = 0; i < q; ++i) ord[i] = i;
    std::sort(ord.begin(), ord.end(), [&](int i, int j) {
        int bi = queries[i].first / block, bj = queries[j].first / block;
        if (bi != bj) return bi < bj;
        // 同块内：偶数块按 r 升序，奇数块按 r 降序（之字形）
        if ((bi & 1) == 0) return queries[i].second < queries[j].second;
        return queries[i].second > queries[j].second;
    });

    int curL = 0, curR = -1;   // 当前窗口 [curL, curR]
    for (int qi : ord) {
        int L = queries[qi].first, R = queries[qi].second;
        // 先扩展（避免先收缩到非法区间：curR 可能 < curL）
        while (curL > L) { --curL; add(curL); }
        while (curR < R) { ++curR; add(curR); }
        while (curL < L) { remove(curL); ++curL; }
        while (curR > R) { remove(curR); --curR; }
        answer(qi);
    }
}

} // namespace better_std
