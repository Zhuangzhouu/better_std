#pragma once
//===== better_std 差分数组：一维 / 二维 =====
#include<vector>
#include<algorithm>
namespace better_std{

// 一维差分数组：区间加 O(1)，整体还原或单点查询
template<typename T>
class std_difference{
    std::vector<T> d;
    int n = 0;
public:
    void build(const std::vector<T>& a){
        n = (int)a.size();
        d.assign(n, T());
        for(int i = 0; i < n; i++)
            d[i] = a[i] - (i > 0 ? a[i-1] : T());
    }
    // 区间 [l, r]（0-based，闭）加 v
    void add(int l, int r, T v){
        if(l > r) return;
        if(l < 0) l = 0;
        if(r >= n) r = n - 1;
        d[l] += v;
        if(r + 1 < n) d[r + 1] -= v;
    }
    // 单点 i 当前值（朴素前缀和）
    T point(int i) const{
        if(i < 0 || i >= n) return T();
        T cur = T();
        for(int k = 0; k <= i; k++) cur += d[k];
        return cur;
    }
    // 全部还原为最终数组（对差分数组求前缀和）
    std::vector<T> get() const{
        std::vector<T> a(n);
        T cur = T();
        for(int i = 0; i < n; i++){ cur += d[i]; a[i] = cur; }
        return a;
    }
    int size() const { return n; }
};

// 二维差分数组：子矩阵加 O(1)，整体还原
template<typename T>
class std_difference_2d{
    std::vector<std::vector<T>> d;
    int rows = 0, cols = 0;
public:
    void build(const std::vector<std::vector<T>>& g){
        rows = (int)g.size();
        cols = rows ? (int)g[0].size() : 0;
        d.assign(rows, std::vector<T>(cols, T()));
        for(int i = 0; i < rows; i++)
            for(int j = 0; j < cols; j++)
                d[i][j] = g[i][j]
                        - (i > 0 ? g[i-1][j] : T())
                        - (j > 0 ? g[i][j-1] : T())
                        + (i > 0 && j > 0 ? g[i-1][j-1] : T());
    }
    // 子矩阵 [x1,y1]..[x2,y2]（0-based，闭）加 v
    void add(int x1, int y1, int x2, int y2, T v){
        if(x1 > x2 || y1 > y2) return;
        if(x1 < 0) x1 = 0;
        if(y1 < 0) y1 = 0;
        if(x2 >= rows) x2 = rows - 1;
        if(y2 >= cols) y2 = cols - 1;
        d[x1][y1] += v;
        if(x2 + 1 < rows) d[x2+1][y1] -= v;
        if(y2 + 1 < cols) d[x1][y2+1] -= v;
        if(x2 + 1 < rows && y2 + 1 < cols) d[x2+1][y2+1] += v;
    }
    // 整体还原为最终矩阵（二维前缀和）
    std::vector<std::vector<T>> get() const{
        std::vector<std::vector<T>> a(rows, std::vector<T>(cols, T()));
        std::vector<std::vector<T>> s(rows + 1, std::vector<T>(cols + 1, T()));
        for(int i = 0; i < rows; i++)
            for(int j = 0; j < cols; j++)
                s[i+1][j+1] = s[i][j+1] + s[i+1][j] - s[i][j] + d[i][j];
        for(int i = 0; i < rows; i++)
            for(int j = 0; j < cols; j++)
                a[i][j] = s[i+1][j+1];
        return a;
    }
    int nrow() const { return rows; }
    int ncol() const { return cols; }
};

} // namespace better_std
