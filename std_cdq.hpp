#pragma once
//===== better_std::std_cdq —— CDQ 分治 =====
//  1) cdq_3d_partial_order : 三维偏序计数
//     给定 n 个点 (x,y,z)，对每点 i 返回「坐标全 <= 的点 j(j!=i) 的个数」
//     （即 j 被 i 支配：x_j<=x_i && y_j<=y_i && z_j<=z_i）。
//     实现：按 x 稳定排序 → CDQ(按下标分治, 按 y 归并, 树状数组按 z 统计)，
//     并对 x/y/z 全相等的点群做对称补正，结果对齐到输入顺序。
//  2) cdq_divide_conquer : 通用 CDQ 分治骨架（merge 回调由用户提供）。
#include<bits/stdc++.h>
namespace better_std {

// 三维偏序结果：count[i] = 坐标全 <= 点 i 的其它点的个数（对齐输入顺序）
struct cdq_3d_result {
    std::vector<long long> count;  // 长度 n，对齐输入顺序
    long long total;               // 所有有序支配对的总数
};

inline cdq_3d_result cdq_3d_partial_order(const std::vector<std::array<int,3>>& pts) {
    int n = (int)pts.size();
    if (n == 0) return {{}, 0};
    // 1) 稳定排序：按 x, 再 y, 再 z, 再输入下标
    std::vector<int> ord(n);
    std::iota(ord.begin(), ord.end(), 0);
    std::stable_sort(ord.begin(), ord.end(), [&](int a, int b) {
        if (pts[a][0] != pts[b][0]) return pts[a][0] < pts[b][0];
        if (pts[a][1] != pts[b][1]) return pts[a][1] < pts[b][1];
        if (pts[a][2] != pts[b][2]) return pts[a][2] < pts[b][2];
        return a < b;
    });
    std::vector<std::array<int,3>> s(n);
    for (int i = 0; i < n; i++) s[i] = pts[ord[i]];

    // 2) 离散化 z
    std::vector<int> zc(n);
    for (int i = 0; i < n; i++) zc[i] = s[i][2];
    std::sort(zc.begin(), zc.end());
    zc.erase(std::unique(zc.begin(), zc.end()), zc.end());
    int m = (int)zc.size();
    auto rz = [&](int v) { return (int)(std::lower_bound(zc.begin(), zc.end(), v) - zc.begin()) + 1; };

    // 3) 树状数组
    std::vector<int> bit(m + 2, 0);
    auto add = [&](int p, int v) { for (; p <= m; p += p & -p) bit[p] += v; };
    auto sum = [&](int p) { int r = 0; for (; p > 0; p -= p & -p) r += bit[p]; return r; };

    std::vector<long long> ans(n, 0);   // 下标对齐「排序后」位置（原始排序序，不随归并改变）
    std::vector<int> idx(n), tmp(n);
    std::iota(idx.begin(), idx.end(), 0);

    std::function<void(int,int)> cdq = [&](int l, int r) {
        if (l >= r) return;
        int mid = (l + r) >> 1;
        cdq(l, mid);
        cdq(mid + 1, r);
        int i = l, j = mid + 1, k = 0;
        while (i <= mid && j <= r) {
            int a = idx[i], b = idx[j];
            if (s[a][1] <= s[b][1]) {          // y 相等也归入「<=」
                add(rz(s[a][2]), 1);
                tmp[k++] = idx[i++];
            } else {
                ans[b] += sum(rz(s[b][2]));
                tmp[k++] = idx[j++];
            }
        }
        while (i <= mid) { add(rz(s[idx[i]][2]), 1); tmp[k++] = idx[i++]; }
        while (j <= r)   { ans[idx[j]] += sum(rz(s[idx[j]][2])); tmp[k++] = idx[j++]; }
        for (i = l; i <= mid; i++) add(rz(s[idx[i]][2]), -1);   // 清 BIT
        for (i = 0; i < k; i++) idx[l + i] = tmp[i];           // 归并只重排 idx，ans 仍以原始排序序为下标
    };
    cdq(0, n - 1);

    // 4) 对 x/y/z 全相等的点群做对称补正：
    //    CDQ 只统计了「排序序中前者 -> 后者」的有序对，需补回「后者 -> 前者」。
    for (int i = 0; i < n; ) {
        int j = i;
        while (j < n && s[j][0] == s[i][0] && s[j][1] == s[i][1] && s[j][2] == s[i][2]) j++;
        int t = j - i;            // 同值点群大小
        for (int k = 0; k < t; k++) ans[i + k] += (long long)(t - 1 - k);  // 补回逆序对等
        i = j;
    }

    // 5) 映射回输入顺序
    cdq_3d_result res;
    res.count.assign(n, 0);
    long long tot = 0;
    for (int p = 0; p < n; p++) { res.count[ord[p]] = ans[p]; tot += ans[p]; }
    res.total = tot;
    return res;
}

// 通用 CDQ 分治骨架：
//   对 a[l..r] 递归二分，先处理左右，再调用 merge(a,l,mid,r) 合并（用户在此做贡献统计与归并）。
//   典型用法：a 已按主维排序，merge 按次维归并并用 BIT 统计第三维。
template<typename T, typename Merge>
void cdq_divide_conquer(std::vector<T>& a, int l, int r, Merge merge) {
    if (l >= r) return;
    int mid = (l + r) >> 1;
    cdq_divide_conquer(a, l, mid, merge);
    cdq_divide_conquer(a, mid + 1, r, merge);
    merge(a, l, mid, r);
}

} // namespace better_std
