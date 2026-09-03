#pragma once
//===== better_std::std_sort —— 各种排序 + 第 k 小选择 =====
//  比较排序: insertion_sort / selection_sort / bubble_sort / shell_sort
//           quick_sort / merge_sort(稳定) / heap_sort
//  非比较排序: counting_sort(整数小值域 O(n+k)) / radix_sort(整数 O(n·w)) / bucket_sort([0,1) 浮点)
//  选择算法: nth_element / quickselect（第 k 小，O(n) 期望）
//  所有比较排序接受 [first,last) 随机访问迭代器区间，支持自定义比较器 cmp(a,b) 表示 a<b。
#include<bits/stdc++.h>
namespace better_std {

//==================== 比较排序 ====================

// 插入排序
template<typename It, typename Cmp>
void insertion_sort(It first, It last, Cmp cmp) {
    using diff = typename std::iterator_traits<It>::difference_type;
    diff n = last - first;
    for (diff i = 1; i < n; i++) {
        auto key = std::move(*(first + i));
        diff j = i;
        while (j > 0 && cmp(key, *(first + (j - 1)))) {
            *(first + j) = std::move(*(first + (j - 1)));
            j--;
        }
        *(first + j) = std::move(key);
    }
}
template<typename It>
void insertion_sort(It first, It last) {
    using T = typename std::iterator_traits<It>::value_type;
    insertion_sort(first, last, std::less<T>());
}

// 选择排序
template<typename It, typename Cmp>
void selection_sort(It first, It last, Cmp cmp) {
    using diff = typename std::iterator_traits<It>::difference_type;
    diff n = last - first;
    for (diff i = 0; i < n; i++) {
        diff m = i;
        for (diff j = i + 1; j < n; j++) if (cmp(*(first + j), *(first + m))) m = j;
        if (m != i) std::iter_swap(first + i, first + m);
    }
}
template<typename It>
void selection_sort(It first, It last) {
    using T = typename std::iterator_traits<It>::value_type;
    selection_sort(first, last, std::less<T>());
}

// 冒泡排序
template<typename It, typename Cmp>
void bubble_sort(It first, It last, Cmp cmp) {
    using diff = typename std::iterator_traits<It>::difference_type;
    diff n = last - first;
    bool swapped = true;
    for (diff i = 0; i < n && swapped; i++) {
        swapped = false;
        for (diff j = 0; j + 1 < n - i; j++)
            if (cmp(*(first + (j + 1)), *(first + j))) {
                std::iter_swap(first + j, first + (j + 1));
                swapped = true;
            }
    }
}
template<typename It>
void bubble_sort(It first, It last) {
    using T = typename std::iterator_traits<It>::value_type;
    bubble_sort(first, last, std::less<T>());
}

// 希尔排序（Knuth 增量 1,4,13,...）
template<typename It, typename Cmp>
void shell_sort(It first, It last, Cmp cmp) {
    using diff = typename std::iterator_traits<It>::difference_type;
    diff n = last - first;
    diff h = 1;
    while (h < n / 3) h = 3 * h + 1;
    for (; h > 0; h /= 3) {
        for (diff i = h; i < n; i++) {
            auto key = std::move(*(first + i));
            diff j = i;
            while (j >= h && cmp(key, *(first + (j - h)))) {
                *(first + j) = std::move(*(first + (j - h)));
                j -= h;
            }
            *(first + j) = std::move(key);
        }
    }
}
template<typename It>
void shell_sort(It first, It last) {
    using T = typename std::iterator_traits<It>::value_type;
    shell_sort(first, last, std::less<T>());
}

// 快速排序（三数取中 + Lomuto 划分，递归）
template<typename It, typename Cmp>
void quick_sort(It first, It last, Cmp cmp) {
    using diff = typename std::iterator_traits<It>::difference_type;
    diff n = last - first;
    if (n <= 1) return;
    // 三数取中：first, mid, last-1 排序，使中值位于 last-1 作为 pivot
    It mid = first + n / 2;
    It e = first + (n - 1);
    if (cmp(*mid, *first)) std::iter_swap(mid, first);
    if (cmp(*e, *first)) std::iter_swap(e, first);
    if (cmp(*e, *mid)) std::iter_swap(e, mid);
    std::iter_swap(mid, e);  // pivot 移到 e
    auto pivot = *e;
    diff i = -1;  // 用下标实现 Lomuto
    for (diff j = 0; j < n - 1; j++) {
        if (cmp(*(first + j), pivot)) {
            i++;
            if (i != j) std::iter_swap(first + i, first + j);
        }
    }
    std::iter_swap(first + (i + 1), e);
    diff p = i + 1;
    quick_sort(first, first + p, cmp);
    quick_sort(first + p + 1, last, cmp);
}
template<typename It>
void quick_sort(It first, It last) {
    using T = typename std::iterator_traits<It>::value_type;
    quick_sort(first, last, std::less<T>());
}

// 归并排序（稳定，自顶向下）
template<typename It, typename Cmp>
void merge_sort(It first, It last, Cmp cmp) {
    using diff = typename std::iterator_traits<It>::difference_type;
    diff n = last - first;
    if (n <= 1) return;
    diff mid = n / 2;
    merge_sort(first, first + mid, cmp);
    merge_sort(first + mid, last, cmp);
    std::vector<typename std::iterator_traits<It>::value_type> tmp(n);
    diff i = 0, j = mid, k = 0;
    while (i < mid && j < n) {
        // 稳定：左优先（仅当右 < 左 才取右）
        if (cmp(*(first + j), *(first + i))) tmp[k++] = std::move(*(first + j++));
        else                                  tmp[k++] = std::move(*(first + i++));
    }
    while (i < mid) tmp[k++] = std::move(*(first + i++));
    while (j < n)   tmp[k++] = std::move(*(first + j++));
    for (diff t = 0; t < n; t++) *(first + t) = std::move(tmp[t]);
}
template<typename It>
void merge_sort(It first, It last) {
    using T = typename std::iterator_traits<It>::value_type;
    merge_sort(first, last, std::less<T>());
}

// 堆排序（手动最大堆，cmp 为 less 时升序）
template<typename It, typename Cmp>
void heap_sort(It first, It last, Cmp cmp) {
    using diff = typename std::iterator_traits<It>::difference_type;
    diff n = last - first;
    if (n <= 1) return;
    auto sift = [&](diff i, diff lim) -> void {
        while (true) {
            diff l = 2 * i + 1, r = 2 * i + 2, m = i;
            if (l < lim && cmp(*(first + m), *(first + l))) m = l;
            if (r < lim && cmp(*(first + m), *(first + r))) m = r;
            if (m == i) break;
            std::iter_swap(first + i, first + m);
            i = m;
        }
    };
    for (diff i = n / 2; i > 0; ) { i--; sift(i, n); }
    for (diff i = n - 1; i > 0; i--) {
        std::iter_swap(first, first + i);
        sift(0, i);   // 堆区缩为 [0,i)
    }
}
template<typename It>
void heap_sort(It first, It last) {
    using T = typename std::iterator_traits<It>::value_type;
    heap_sort(first, last, std::less<T>());
}

//==================== 非比较排序 ====================

// 计数排序：整数（或整数可比较）类型，值域较小；值域过大时回退 std::sort。
template<typename T>
void counting_sort(std::vector<T>& a) {
    if (a.empty()) return;
    T mn = *std::min_element(a.begin(), a.end());
    T mx = *std::max_element(a.begin(), a.end());
    uint64_t rng = (uint64_t)mx - (uint64_t)mn + 1;
    if (rng > (uint64_t)5'000'000) { std::sort(a.begin(), a.end()); return; }
    std::vector<uint64_t> cnt(rng, 0);
    for (const auto& x : a) cnt[(uint64_t)x - (uint64_t)mn]++;
    size_t p = 0;
    for (uint64_t v = 0; v < rng; v++)
        for (uint64_t c = 0; c < cnt[v]; c++) a[p++] = (T)(v + (uint64_t)mn);
}

// 基数排序（LSD，按字节，支持有符号/无符号整数；O(n·w)）
template<typename T>
void radix_sort(std::vector<T>& a) {
    size_t n = a.size();
    if (n <= 1) return;
    T mn = a[0];
    for (const auto& x : a) if (x < mn) mn = x;
    std::vector<uint64_t> keys(n), tmp(n);
    for (size_t i = 0; i < n; i++) keys[i] = (uint64_t)a[i] - (uint64_t)mn;
    for (int b = 0; b < 8; b++) {
        int cnt[256] = {0};
        int shift = b * 8;
        for (size_t i = 0; i < n; i++) cnt[(keys[i] >> shift) & 255]++;
        for (int i = 1; i < 256; i++) cnt[i] += cnt[i - 1];
        for (int i = (int)n - 1; i >= 0; i--) {
            int d = (keys[i] >> shift) & 255;
            tmp[--cnt[d]] = keys[i];
        }
        for (size_t i = 0; i < n; i++) keys[i] = tmp[i];
    }
    for (size_t i = 0; i < n; i++) a[i] = (T)(keys[i] + (uint64_t)mn);
}

// 桶排序：元素需落在 [lo,hi) 区间（经典用于 [0,1) 浮点），桶内 std::sort。
inline void bucket_sort(std::vector<double>& a, double lo = 0.0, double hi = 1.0) {
    size_t n = a.size();
    if (n <= 1) return;
    int nb = (int)n;
    std::vector<std::vector<double>> bk(nb);
    double span = hi - lo;
    for (double x : a) {
        int idx = (int)((x - lo) / span * nb);
        if (idx < 0) idx = 0;
        if (idx >= nb) idx = nb - 1;
        bk[idx].push_back(x);
    }
    size_t p = 0;
    for (int i = 0; i < nb; i++) {
        std::sort(bk[i].begin(), bk[i].end());
        for (double x : bk[i]) a[p++] = x;
    }
}

//==================== 选择算法（第 k 小） ====================

// 将 [first,last) 中第 k 小（0-indexed）置于位置 first+k，并保证其左侧均 <=、右侧均 >=。
template<typename It, typename Cmp>
void nth_element(It first, It last, size_t k, Cmp cmp) {
    using diff = typename std::iterator_traits<It>::difference_type;
    diff n = last - first;
    if (n <= 1 || (diff)k >= n) return;
    // 三数取中 + Lomuto 划分，返回 pivot 最终下标 p（相对 first）
    auto partition = [&](It l, It r) -> diff {
        diff len = r - l;
        It mid = l + len / 2;
        It e = l + (len - 1);
        if (cmp(*mid, *l)) std::iter_swap(mid, l);
        if (cmp(*e, *l)) std::iter_swap(e, l);
        if (cmp(*e, *mid)) std::iter_swap(e, mid);
        std::iter_swap(mid, e);
        auto pivot = *e;
        diff i = -1;
        for (diff j = 0; j < len - 1; j++)
            if (cmp(*(l + j), pivot)) {
                i++;
                if (i != j) std::iter_swap(l + i, l + j);
            }
        std::iter_swap(l + (i + 1), e);
        return i + 1;
    };
    It l = first, r = last;
    size_t kk = k;
    while (true) {
        diff len = r - l;
        if (len <= 1) break;
        diff p = partition(l, r);
        if ((diff)p == (diff)kk) return;
        else if ((diff)p > (diff)kk) { r = l + p; }
        else { kk -= (size_t)p + 1; l = l + p + 1; }
    }
}
template<typename It>
void nth_element(It first, It last, size_t k) {
    using T = typename std::iterator_traits<It>::value_type;
    nth_element(first, last, k, std::less<T>());
}

// quickselect：返回第 k 小的值（0-indexed）
template<typename It, typename Cmp>
typename std::iterator_traits<It>::value_type
quickselect(It first, It last, size_t k, Cmp cmp) {
    nth_element(first, last, k, cmp);
    return *(first + k);
}
template<typename It>
typename std::iterator_traits<It>::value_type
quickselect(It first, It last, size_t k) {
    using T = typename std::iterator_traits<It>::value_type;
    return quickselect(first, last, k, std::less<T>());
}

} // namespace better_std
