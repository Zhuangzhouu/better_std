#pragma once
//===== better_std 离散化（坐标压缩） =====
//把任意可比较类型的序列映射到 0..m-1 的连续整数下标（保留大小顺序），
//是离线/数组化处理的常用预处理。区间默认 [0, m-1]。
#include <vector>
#include <algorithm>
#include <map>

namespace better_std {

// 对 values 做去重排序，返回有序唯一值列表（即"值域 -> 下标"的反向映射）
template<typename T>
std::vector<T> compress(const std::vector<T>& values) {
    std::vector<T> uniq(values.begin(), values.end());
    std::sort(uniq.begin(), uniq.end());
    uniq.erase(std::unique(uniq.begin(), uniq.end()), uniq.end());
    return uniq;
}

// 查询 x 在有序唯一值列表中的下标（等价于 lower_bound 下标），不存在则为插入位置
template<typename T>
int rank_of(const std::vector<T>& uniq, const T& x) {
    return (int)(std::lower_bound(uniq.begin(), uniq.end(), x) - uniq.begin());
}

// 原地离散化：把 arr 中每个元素替换为它在自身值域内的下标（0-based，保留顺序）
// 返回映射表（uniq[i] 是第 i 个离散值），arr 被原地改写
template<typename T>
std::vector<T> discretize_inplace(std::vector<T>& arr) {
    std::vector<T> uniq = compress(arr);
    for (auto& v : arr) v = (T)rank_of(uniq, v);   // 替换为下标（秩），非原值
    return uniq;
}

// 对一组成对元素（如区间端点、坐标点）整体离散化，返回统一映射表
template<typename T>
std::vector<T> discretize_pairs(const std::vector<std::pair<T,T>>& pairs) {
    std::vector<T> all;
    all.reserve(pairs.size() * 2);
    for (const auto& p : pairs) { all.push_back(p.first); all.push_back(p.second); }
    return compress(all);
}

} // namespace better_std
