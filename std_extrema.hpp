#pragma once
//===== better_std::std_extrema —— 离散最值 / 前缀（后缀）极值结构 =====
//  prefix_extrema<T>     : 一维前缀极值，O(n) 预处理 O(1) 查询 [0,k) 最值及首个/末个最值下标
//  suffix_extrema<T>     : 一维后缀极值，O(1) 查询 [k,n)
//  extrema_stack<T>      : 带 min/max 的栈，push/pop O(1)
//  prefix_extrema_2d<T>  : 二维前缀极值，O(1) 查询 [0,r)×[0,c) 最值及位置
#include <vector>
#include <algorithm>
#include <cstddef>
#include <iterator>
#include <utility>
namespace better_std {

//==================== 一维前缀极值 ====================
template<class T>
struct prefix_extrema {
    std::vector<T> mn, mx;
    std::vector<int> mn_f, mx_f, mn_l, mx_l;   // 首个/末个最值下标
    prefix_extrema() {}
    template<class It> prefix_extrema(It first, It last) { build(first, last); }
    template<class It> void build(It first, It last) {
        int n = (int)std::distance(first, last);
        mn.assign(n, T()); mx.assign(n, T());
        mn_f.assign(n, 0); mx_f.assign(n, 0); mn_l.assign(n, 0); mx_l.assign(n, 0);
        for (int i = 0; i < n; i++) {
            T v = *(first + i);
            if (i == 0) {
                mn[0] = mx[0] = v;
                mn_f[0] = mx_f[0] = mn_l[0] = mx_l[0] = 0;
            } else {
                mn[i] = std::min(mn[i - 1], v);
                mx[i] = std::max(mx[i - 1], v);
                mn_f[i] = (v < mn[i - 1]) ? i : mn_f[i - 1];   // 首个最小值
                mx_f[i] = (v > mx[i - 1]) ? i : mx_f[i - 1];   // 首个最大值
                mn_l[i] = (v <= mn[i - 1]) ? i : mn_l[i - 1];  // 末个最小值
                mx_l[i] = (v >= mx[i - 1]) ? i : mx_l[i - 1];  // 末个最大值
            }
        }
    }
    // 前缀 [0, k)
    T min_value(int k) const { return mn[k - 1]; }
    T max_value(int k) const { return mx[k - 1]; }
    int argmin_first(int k) const { return mn_f[k - 1]; }
    int argmax_first(int k) const { return mx_f[k - 1]; }
    int argmin_last(int k) const { return mn_l[k - 1]; }
    int argmax_last(int k) const { return mx_l[k - 1]; }
};

//==================== 一维后缀极值 ====================
template<class T>
struct suffix_extrema {
    std::vector<T> mn, mx;
    std::vector<int> mn_f, mx_f, mn_l, mx_l;
    suffix_extrema() {}
    template<class It> suffix_extrema(It first, It last) { build(first, last); }
    template<class It> void build(It first, It last) {
        int n = (int)std::distance(first, last);
        mn.assign(n, T()); mx.assign(n, T());
        mn_f.assign(n, 0); mx_f.assign(n, 0); mn_l.assign(n, 0); mx_l.assign(n, 0);
        for (int i = n - 1; i >= 0; i--) {
            T v = *(first + i);
            if (i == n - 1) {
                mn[i] = mx[i] = v;
                mn_f[i] = mx_f[i] = mn_l[i] = mx_l[i] = i;
            } else {
                mn[i] = std::min(mn[i + 1], v);
                mx[i] = std::max(mx[i + 1], v);
                // 后缀 [i,n)：最左（first）出现与最右（last）出现的判定方向与前缀相反
                mn_f[i] = (v <= mn[i + 1]) ? i : mn_f[i + 1];  // 首个（最左）最小值
                mx_f[i] = (v >= mx[i + 1]) ? i : mx_f[i + 1];  // 首个（最左）最大值
                mn_l[i] = (v <  mn[i + 1]) ? i : mn_l[i + 1];  // 末个（最右）最小值
                mx_l[i] = (v >  mx[i + 1]) ? i : mx_l[i + 1];  // 末个（最右）最大值
            }
        }
    }
    // 后缀 [k, n)
    T min_value(int k) const { return mn[k]; }
    T max_value(int k) const { return mx[k]; }
    int argmin_first(int k) const { return mn_f[k]; }
    int argmax_first(int k) const { return mx_f[k]; }
    int argmin_last(int k) const { return mn_l[k]; }
    int argmax_last(int k) const { return mx_l[k]; }
};

//==================== 带 min/max 的栈 ====================
template<class T>
struct extrema_stack {
    std::vector<T> st, mn, mx;
    void push(const T& v) {
        mn.push_back(st.empty() ? v : std::min(mn.back(), v));
        mx.push_back(st.empty() ? v : std::max(mx.back(), v));
        st.push_back(v);
    }
    void pop() {
        if (st.empty()) return;
        st.pop_back(); mn.pop_back(); mx.pop_back();
    }
    T top() const { return st.back(); }
    T min_value() const { return mn.back(); }
    T max_value() const { return mx.back(); }
    bool empty() const { return st.empty(); }
    std::size_t size() const { return st.size(); }
    void clear() { st.clear(); mn.clear(); mx.clear(); }
};

//==================== 二维前缀极值（前缀矩形 [0,r)×[0,c)）=====================
template<class T>
struct prefix_extrema_2d {
    int R, C;
    std::vector<std::vector<T>> mn, mx;
    std::vector<std::vector<std::pair<int, int>>> mn_arg, mx_arg;
    prefix_extrema_2d() : R(0), C(0) {}
    template<class Mat> prefix_extrema_2d(const Mat& a) { build(a); }
    template<class Mat> void build(const Mat& a) {
        R = (int)a.size();
        C = R ? (int)a[0].size() : 0;
        mn.assign(R, std::vector<T>(C));
        mx.assign(R, std::vector<T>(C));
        mn_arg.assign(R, std::vector<std::pair<int, int>>(C));
        mx_arg.assign(R, std::vector<std::pair<int, int>>(C));
        for (int i = 0; i < R; i++)
            for (int j = 0; j < C; j++) {
                T v = a[i][j];
                if (i == 0 && j == 0) {
                    mn[0][0] = mx[0][0] = v;
                    mn_arg[0][0] = mx_arg[0][0] = {0, 0};
                    continue;
                }
                T bmn = v, bmx = v;
                std::pair<int, int> bmn_arg = {i, j}, bmx_arg = {i, j};
                if (i > 0 && mn[i - 1][j] < bmn) { bmn = mn[i - 1][j]; bmn_arg = mn_arg[i - 1][j]; }
                if (j > 0 && mn[i][j - 1] < bmn) { bmn = mn[i][j - 1]; bmn_arg = mn_arg[i][j - 1]; }
                if (i > 0 && mx[i - 1][j] > bmx) { bmx = mx[i - 1][j]; bmx_arg = mx_arg[i - 1][j]; }
                if (j > 0 && mx[i][j - 1] > bmx) { bmx = mx[i][j - 1]; bmx_arg = mx_arg[i][j - 1]; }
                mn[i][j] = bmn; mx[i][j] = bmx;
                mn_arg[i][j] = bmn_arg; mx_arg[i][j] = bmx_arg;
            }
    }
    // 查询前缀矩形 [0,r)×[0,c)
    T min_value(int r, int c) const { return mn[r - 1][c - 1]; }
    T max_value(int r, int c) const { return mx[r - 1][c - 1]; }
    std::pair<int, int> argmin(int r, int c) const { return mn_arg[r - 1][c - 1]; }
    std::pair<int, int> argmax(int r, int c) const { return mx_arg[r - 1][c - 1]; }
};

} // namespace better_std
