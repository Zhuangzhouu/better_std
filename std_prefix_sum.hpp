#pragma once
//===== better_std 一维 / 二维前缀和 =====
#include<vector>
#include<algorithm>
namespace better_std{

// 一维前缀和（只读，O(1) 区间和）
template<typename T>
class std_prefix_sum{
    std::vector<T> pre;
public:
    void build(const std::vector<T>& a){
        int n = (int)a.size();
        pre.assign(n+1, T());
        for(int i = 0; i < n; i++) pre[i+1] = pre[i] + a[i];
    }
    // 闭区间 [l, r] 和（0-based）；越界或 l>r 返回 T()
    T query(int l, int r) const{
        int n = (int)pre.size() - 1;
        if(l > r || l < 0 || r >= n) return T();
        return pre[r+1] - pre[l];
    }
    int size() const { return (int)pre.size() - 1; }
};

// 二维前缀和（只读，O(1) 子矩阵和）
template<typename T>
class std_prefix_sum_2d{
    std::vector<std::vector<T>> pre;
    int rows = 0, cols = 0;
public:
    void build(const std::vector<std::vector<T>>& g){
        rows = (int)g.size();
        if(rows == 0){ cols = 0; pre.clear(); return; }
        cols = (int)g[0].size();
        pre.assign(rows+1, std::vector<T>(cols+1, T()));
        for(int i = 0; i < rows; i++)
            for(int j = 0; j < cols; j++)
                pre[i+1][j+1] = pre[i][j+1] + pre[i+1][j] - pre[i][j] + g[i][j];
    }
    // 闭区间子矩阵 [x1,y1]..[x2,y2] 和（0-based）；越界或 x1>x2/y1>y2 返回 T()
    T query(int x1, int y1, int x2, int y2) const{
        if(x1 > x2 || y1 > y2) return T();
        if(x1 < 0 || y1 < 0 || x2 >= rows || y2 >= cols) return T();
        return pre[x2+1][y2+1] - pre[x1][y2+1] - pre[x2+1][y1] + pre[x1][y1];
    }
    int nrow() const { return rows; }
    int ncol() const { return cols; }
};

} // namespace better_std
