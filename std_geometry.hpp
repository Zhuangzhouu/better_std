#pragma once
//===== better_std::std_geometry —— 计算几何基础 =====
//  point2<T>             : 二维点/向量，支持四则/点积/叉积/长度/极角/旋转
//  cross(a,b,c)/orient   : 三点点积叉积与转向
//  convex_hull           : Andrew 单调链凸包（可选保留共线边界点）
//  polygon_area          : 鞋带公式
//  on_segment / segment_intersect : 线段相交判定
//  point_in_polygon      : 射线法点在多边形内
//  rectangle_union_area  : 扫描线 + 离散化 + 覆盖长度线段树（矩形面积并）
#include <vector>
#include <algorithm>
#include <cmath>
#include <array>
#include <functional>
#include <cstdint>
namespace better_std {

//==================== 二维点 / 向量 ====================
template<class T = long long>
struct point2 {
    T x, y;
    point2() : x(0), y(0) {}
    point2(T x_, T y_) : x(x_), y(y_) {}
    point2 operator+(const point2& o) const { return {x + o.x, y + o.y}; }
    point2 operator-(const point2& o) const { return {x - o.x, y - o.y}; }
    point2 operator*(T k) const { return {x * k, y * k}; }
    point2 operator/(T k) const { return {x / k, y / k}; }
    bool operator==(const point2& o) const { return x == o.x && y == o.y; }
    bool operator!=(const point2& o) const { return !(*this == o); }
    bool operator<(const point2& o) const { return x != o.x ? x < o.x : y < o.y; }
    T dot(const point2& o) const { return x * o.x + y * o.y; }
    T cross(const point2& o) const { return x * o.y - y * o.x; }
    long double len2() const { return (long double)x * x + (long double)y * y; }
    long double len() const { return std::sqrt(len2()); }
    long double angle() const { return std::atan2((long double)y, (long double)x); }  // [-pi, pi]
    point2<long double> rotate(long double a) const {
        long double c = std::cos(a), s = std::sin(a);
        return { (long double)x * c - (long double)y * s, (long double)x * s + (long double)y * c };
    }
};

// (b-a) × (c-a)
template<class T> T cross(const point2<T>& a, const point2<T>& b, const point2<T>& c) {
    return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
}
// (b-a) · (c-a)
template<class T> T dot(const point2<T>& a, const point2<T>& b, const point2<T>& c) {
    return (b.x - a.x) * (c.x - a.x) + (b.y - a.y) * (c.y - a.y);
}
// 转向：>0 逆时针（c 在 ab 左侧），<0 顺时针，=0 共线
template<class T> int orient(const point2<T>& a, const point2<T>& b, const point2<T>& c) {
    T v = cross(a, b, c);
    return (v > 0) - (v < 0);
}

//==================== 凸包（Andrew 单调链）=====================
// 返回逆时针凸包。include_collinear=false 去除边上共线点；=true 保留全部边界点。
template<class T>
std::vector<point2<T>> convex_hull(std::vector<point2<T>> p, bool include_collinear = false) {
    int n = (int)p.size();
    if (n <= 1) return p;
    std::sort(p.begin(), p.end());
    std::vector<point2<T>> h(2 * n);
    int k = 0;
    auto push_point = [&](const point2<T>& v, int min_k) {
        while (k > min_k) {
            T cr = cross(h[k - 2], h[k - 1], v);
            bool bad = include_collinear ? (cr < 0) : (cr <= 0);
            if (bad) k--;
            else break;
        }
        h[k++] = v;
    };
    for (int i = 0; i < n; i++) push_point(p[i], 1);        // 下壳：需 ≥2 点才能判转向
    int lower = k;
    for (int i = n - 2; i >= 0; i--) push_point(p[i], lower); // 上壳：不得低于下壳终点
    h.resize(k - 1);   // 去掉末尾重复的起点
    // 全共线（且不含共线点）时只剩两端点：h 可能被压成单点，补回最右点
    if (!include_collinear && (int)h.size() == 1 && p[0] != p[n - 1]) h.push_back(p[n - 1]);
    return h;
}

//==================== 多边形面积（鞋带公式，取绝对值）=====================
template<class T>
long double polygon_area(const std::vector<point2<T>>& p) {
    long double s = 0;
    int n = (int)p.size();
    for (int i = 0; i < n; i++) {
        int j = (i + 1) % n;
        s += (long double)p[i].x * p[j].y - (long double)p[i].y * p[j].x;
    }
    return std::fabs(s) / 2;
}

//==================== 线段相交 ====================
template<class T>
bool on_segment(const point2<T>& a, const point2<T>& b, const point2<T>& p) {
    // p 在 ab 上（含端点），前提 cross(a,b,p)==0
    return std::min(a.x, b.x) <= p.x && p.x <= std::max(a.x, b.x) &&
           std::min(a.y, b.y) <= p.y && p.y <= std::max(a.y, b.y);
}
template<class T>
bool segment_intersect(const point2<T>& a, const point2<T>& b,
                       const point2<T>& c, const point2<T>& d) {
    T c1 = cross(a, b, c), c2 = cross(a, b, d);
    T c3 = cross(c, d, a), c4 = cross(c, d, b);
    if (((c1 > 0 && c2 < 0) || (c1 < 0 && c2 > 0)) &&
        ((c3 > 0 && c4 < 0) || (c3 < 0 && c4 > 0))) return true;
    if (c1 == 0 && on_segment(a, b, c)) return true;
    if (c2 == 0 && on_segment(a, b, d)) return true;
    if (c3 == 0 && on_segment(c, d, a)) return true;
    if (c4 == 0 && on_segment(c, d, b)) return true;
    return false;
}

//==================== 点在多边形内（射线法）=====================
template<class T>
bool point_in_polygon(const point2<T>& p, const std::vector<point2<T>>& poly,
                      bool include_boundary = false) {
    int n = (int)poly.size();
    if (include_boundary)
        for (int i = 0; i < n; i++) {
            int j = (i + 1) % n;
            if (cross(poly[i], poly[j], p) == 0 && on_segment(poly[i], poly[j], p)) return true;
        }
    bool in = false;
    for (int i = 0, j = n - 1; i < n; j = i++) {
        const point2<T>& a = poly[i];
        const point2<T>& b = poly[j];
        if ((a.y > p.y) != (b.y > p.y)) {
            long double xint = (long double)a.x +
                ((long double)(p.y - a.y) / (b.y - a.y)) * ((long double)(b.x - a.x));
            if ((long double)p.x < xint) in = !in;
        }
    }
    return in;
}

//==================== 矩形面积并（扫描线）=====================
// rect: {x1, y1, x2, y2}（左下到右上，允许退化矩形自动忽略）；坐标为整数，返回整数面积。
long long rectangle_union_area(std::vector<std::array<long long, 4>> rects) {
    struct event { long long x, y1, y2; int delta; };
    std::vector<event> evs;
    std::vector<long long> ys;
    for (auto& r : rects) {
        if (r[0] >= r[2] || r[1] >= r[3]) continue;   // 退化矩形忽略
        ys.push_back(r[1]); ys.push_back(r[3]);
        evs.push_back({r[0], r[1], r[3], +1});
        evs.push_back({r[2], r[1], r[3], -1});
    }
    if (evs.empty()) return 0;
    std::sort(ys.begin(), ys.end());
    ys.erase(std::unique(ys.begin(), ys.end()), ys.end());
    int m = (int)ys.size() - 1;   // 区间 [ys[i], ys[i+1])，共 m 个
    std::vector<int> cnt(4 * m + 4, 0);
    std::vector<long long> len(4 * m + 4, 0);
    std::function<void(int, int, int, int, int, int)> upd =
        [&](int node, int l, int r, int ql, int qr, int delta) {
            // 区间节点 l..r 对应 ys[l..r+1]
            if (ql <= l && r <= qr) { cnt[node] += delta; }
            else {
                int mid = (l + r) >> 1;
                if (ql <= mid) upd(node << 1, l, mid, ql, qr, delta);
                if (qr > mid) upd(node << 1 | 1, mid + 1, r, ql, qr, delta);
            }
            if (cnt[node] > 0) len[node] = ys[r + 1] - ys[l];
            else if (l == r) len[node] = 0;
            else len[node] = len[node << 1] + len[node << 1 | 1];
        };
    auto idx = [&](long long v) { return (int)(std::lower_bound(ys.begin(), ys.end(), v) - ys.begin()); };
    std::sort(evs.begin(), evs.end(), [](const event& a, const event& b) { return a.x < b.x; });
    long long area = 0;
    for (size_t i = 0; i < evs.size(); ) {
        long long x = evs[i].x;
        while (i < evs.size() && evs[i].x == x) {
            int l = idx(evs[i].y1), r = idx(evs[i].y2) - 1;
            upd(1, 0, m - 1, l, r, evs[i].delta);
            i++;
        }
        long long nx = (i < evs.size()) ? evs[i].x : evs.back().x;
        area += len[1] * (nx - x);
    }
    return area;
}

} // namespace better_std
