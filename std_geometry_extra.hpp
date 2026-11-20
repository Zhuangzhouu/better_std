#ifndef BETTER_STD_STD_GEOMETRY_EXTRA_HPP
#define BETTER_STD_STD_GEOMETRY_EXTRA_HPP

//===== 计算几何进阶 =====
//  minimal_enclosing_circle(pts) : 最小圆覆盖（随机化 O(n) 期望），返回 {圆心, 半径}
//  half_plane_intersection(lines) : 半平面交，返回凸多边形顶点（逆时针）
//  circle_line_intersect / 辅助
#include<vector>
#include<algorithm>
#include<cmath>
#include<random>
#include<utility>
namespace better_std{

namespace geom_extra_detail{
    const double EPS = 1e-10;
    struct P{
        double x, y;
        P() : x(0), y(0) {}
        P(double x_, double y_) : x(x_), y(y_) {}
        P operator+(const P& o) const { return P(x+o.x, y+o.y); }
        P operator-(const P& o) const { return P(x-o.x, y-o.y); }
        P operator*(double k) const { return P(x*k, y*k); }
        P operator/(double k) const { return P(x/k, y/k); }
    };
    inline double dot(P a, P b){ return a.x*b.x + a.y*b.y; }
    inline double cross(P a, P b){ return a.x*b.y - a.y*b.x; }
    inline double norm2(P a){ return dot(a,a); }
    inline double dist(P a, P b){ return std::sqrt(norm2(a-b)); }
    struct Circle{ P c; double r; Circle():c(),r(0){} Circle(P c_,double r_):c(c_),r(r_){} };

    // 由三点确定外接圆；共线时返回覆盖其中最远两点的圆
    inline Circle circumcircle(P a, P b, P c){
        P ab = b - a, ac = c - a;
        double d = 2.0 * cross(ab, ac);
        if (std::fabs(d) < EPS) {
            // 退化：取最远两点为直径
            double dab = norm2(a-b), dac = norm2(a-c), dbc = norm2(b-c);
            P p = a, q = b;
            if (dac > dab && dac > dbc) { p = a; q = c; }
            else if (dbc > dab) { p = b; q = c; }
            return Circle((p+q)/2.0, dist(p,q)/2.0);
        }
        double abn = norm2(ab), acn = norm2(ac);
        P perpab(-ab.y, ab.x), perpac(-ac.y, ac.x);
        P o = a + (perpab*acn - perpac*abn) * (1.0/d);
        return Circle(o, dist(o, a));
    }
    inline Circle circle_from2(P a, P b){ return Circle((a+b)/2.0, dist(a,b)/2.0); }

    // 半平面：a*x + b*y + c <= 0 表示保留侧
    struct Line{
        P p, v;                       // 线上一点 + 方向向量
        Line() {}
        Line(P p_, P v_) : p(p_), v(v_) {}
    };
    inline bool leftOf(const Line& l, P q){ return cross(l.v, q - l.p) > EPS; }
    inline bool onLine(const Line& l, P q){ return std::fabs(cross(l.v, q - l.p)) <= EPS; }
    inline P line_intersect(const Line& a, const Line& b){
        P u = a.p - b.p;
        double t = cross(b.v, u) / cross(a.v, b.v);
        return a.p + a.v * t;
    }
}

//—— 最小圆覆盖：随机增量 O(n) 期望 ——
inline std::pair<std::pair<double,double>,double> minimal_enclosing_circle(std::vector<std::pair<double,double>> pts){
    using namespace geom_extra_detail;
    std::vector<P> p;
    p.reserve(pts.size());
    for (auto& q : pts) p.push_back(P(q.first, q.second));
    if (p.empty()) return {{0,0}, 0};
    std::mt19937 rng(712367);
    std::shuffle(p.begin(), p.end(), rng);
    Circle c(p[0], 0);
    for (int i = 0; i < (int)p.size(); i++) {
        if (dist(c.c, p[i]) <= c.r + EPS) continue;
        c = circle_from2(p[0], p[i]);
        for (int j = 1; j < i; j++) {
            if (dist(c.c, p[j]) <= c.r + EPS) continue;
            c = circle_from2(p[i], p[j]);
            for (int k = 0; k < j; k++) {
                if (dist(c.c, p[k]) <= c.r + EPS) continue;
                c = circumcircle(p[i], p[j], p[k]);
            }
        }
    }
    return {{c.c.x, c.c.y}, c.r};
}

//—— 半平面交：lines 为 {线上点(x,y), 方向向量(dx,dy)}，保留方向左侧 ——
// 返回一个凸多边形（逆时针），若无解返回空。
inline std::vector<std::pair<double,double>> half_plane_intersection(
        const std::vector<std::pair<std::pair<double,double>,std::pair<double,double>>>& lines){
    using namespace geom_extra_detail;
    std::vector<Line> ls;
    for (auto& e : lines) ls.push_back(Line(P(e.first.first, e.first.second), P(e.second.first, e.second.second)));
    // 用极角排序（方向向量），同角保留更靠左的
    std::sort(ls.begin(), ls.end(), [](const Line& a, const Line& b){
        double ca = std::atan2(a.v.y, a.v.x), cb = std::atan2(b.v.y, b.v.x);
        if (std::fabs(ca-cb) > EPS) return ca < cb;
        return cross(a.v, b.p - a.p) < 0;   // b 在 a 右侧则 a 先
    });
    std::vector<Line> dq(ls.size()+1);
    std::vector<P> pt(ls.size()+1);      // pt[i] = dq[i-1] 与 dq[i] 的交点
    int l = 0, r = -1;                   // deque 区间 [l,r]
    for (size_t i = 0; i < ls.size(); i++) {
        Line cur = ls[i];
        // 同方向直线：保留更严格（更靠左/内侧）的
        if (r >= l && std::fabs(cross(dq[r].v, cur.v)) < EPS) {
            if (leftOf(cur, pt[r]) || onLine(cur, pt[r])) continue; // 新线更松，丢弃
            r--;                                                     // 否则替换
        }
        while (r - l + 1 >= 2 && !leftOf(cur, pt[r])) r--;
        while (r - l + 1 >= 2 && !leftOf(cur, pt[l+1])) l++;
        if (r - l + 1 >= 1) pt[r+1] = line_intersect(dq[r], cur);
        dq[++r] = cur;
    }
    while (r - l + 1 >= 2 && !leftOf(dq[l], pt[r])) r--;
    if (r - l + 1 < 3) return {};
    // 输出每条直线与其下一条（环形）的交点
    std::vector<std::pair<double,double>> res;
    for (int i = l; i <= r; i++) {
        int ni = (i == r) ? l : i+1;
        P p = line_intersect(dq[i], dq[ni]);
        res.push_back({p.x, p.y});
    }
    return res;
}

} // namespace better_std
#endif // BETTER_STD_STD_GEOMETRY_EXTRA_HPP
