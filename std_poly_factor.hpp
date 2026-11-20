#ifndef BETTER_STD_STD_POLY_FACTOR_HPP
#define BETTER_STD_STD_POLY_FACTOR_HPP

//===== std_poly_factor.hpp：任意多元多项式在有理数域 Q 上的不可约因式分解 =====
// 从零实现（不依赖第三方库）：BigInt 精确大整数 → Kronecker 代入法（一元）
//   → 多变量 base-M 编码压成二元 → 单变量插值恢复因式 → 分组解码 + 精确乘积校验。
// 用法：
//   auto r = better_std::factor_polynomial("x^4-1");
//   std::cout << r.expression();        // (x - 1) * (x + 1) * (x^2 + 1)
//   r.constant / r.factors             // 整数 content / 各不可约因式字符串
// 结果为 Q 上不可约因式之积（高斯引理）。算法最坏指数级，内置搜索预算与 3 秒
//   墙钟保护；超预算时保守返回原式（视为不可约），绝不给出错误分解。
// 表达式支持 + - * ^ ( )、隐式乘法、单字母变量与整数系数；# 后为注释。
// 注意：内部使用命名空间级状态，请单线程调用。

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <functional>
#include <cstdlib>
#include <ctime>
#include <cctype>
#include <climits>
#include <utility>
#include <sstream>

namespace better_std {
namespace poly_factor_detail {
using namespace std;

// C++17 用 inline 变量（跨 TU 共享）；C++11/14 回退为 static（内部链接，每个 TU 独立副本）。
// 这些状态在每次 factor 入口处都会重置（见 factorMulti），多 TU 下语义一致。
#if defined(__cplusplus) && __cplusplus >= 201703L
inline vector<char> gVarNames;
inline int gNVars = 0;
// 搜索预算：Kronecker / 多元插值都是指数级的，超预算就保守返回“不可约”，保证不会卡死
inline long long gWork = 0; inline bool gAbort = false;
inline clock_t gStart = 0;
#else
static vector<char> gVarNames;
static int gNVars = 0;
static long long gWork = 0; static bool gAbort = false;
static clock_t gStart = 0;
#endif
const long long WORK_LIMIT = 120000;
const double TIME_LIMIT_SEC = 3.0;
inline bool overTime() { return ((gWork & 63) == 0) && ((double)(clock() - gStart) / CLOCKS_PER_SEC > TIME_LIMIT_SEC); }

// ===================== 大整数 BigInt (base 1e9) =====================
struct BigInt {
    int sign = 0;
    vector<long long> d;
    static const long long BASE = 1000000000;
    static const int WIDTH = 9;
    BigInt() { d = {0}; }
    BigInt(long long v) { if (v == 0) { sign = 0; d = {0}; return; } sign = v < 0 ? -1 : 1; long long x = v < 0 ? -v : v; while (x) { d.push_back(x % BASE); x /= BASE; } }
    void norm() { while (d.size() > 1 && d.back() == 0) d.pop_back(); if (d.empty()) d = {0}; if (d.size() == 1 && d[0] == 0) sign = 0; if (sign == 0) d = {0}; }
    int cmpAbs(const BigInt& o) const {
        if (d.size() != o.d.size()) return d.size() < o.d.size() ? -1 : 1;
        for (int i = (int)d.size() - 1; i >= 0; i--) if (d[i] != o.d[i]) return d[i] < o.d[i] ? -1 : 1;
        return 0;
    }
    bool operator<(const BigInt& o) const { if (sign != o.sign) return sign < o.sign; if (sign == 0) return false; int c = cmpAbs(o); return sign > 0 ? c < 0 : c > 0; }
    bool operator==(const BigInt& o) const { return sign == o.sign && cmpAbs(o) == 0; }
    bool operator!=(const BigInt& o) const { return !(*this == o); }
    BigInt abs() const { BigInt r = *this; if (r.sign < 0) r.sign = 1; return r; }
};
inline BigInt operator+(const BigInt& a, const BigInt& b) {
    if (a.sign == 0) return b;
    if (b.sign == 0) return a;
    if (a.sign == b.sign) { BigInt r; r.sign = a.sign; r.d.clear(); long long c = 0; size_t n = std::max(a.d.size(), b.d.size()); for (size_t i = 0; i < n || c; i++) { long long s = c + (i < a.d.size() ? a.d[i] : 0) + (i < b.d.size() ? b.d[i] : 0); if (s >= BigInt::BASE) { s -= BigInt::BASE; c = 1; } else c = 0; r.d.push_back(s); } r.norm(); return r; }
    BigInt A = a.abs(), B = b.abs(); int cmp = A.cmpAbs(B); if (cmp == 0) return BigInt(0);
    BigInt r; r.sign = (cmp > 0 ? a.sign : b.sign); r.d.clear(); const BigInt& big = cmp > 0 ? A : B; const BigInt& sm = cmp > 0 ? B : A; long long borrow = 0;
    for (size_t i = 0; i < big.d.size(); i++) { long long s = big.d[i] - borrow - (i < sm.d.size() ? sm.d[i] : 0); if (s < 0) { s += BigInt::BASE; borrow = 1; } else borrow = 0; r.d.push_back(s); } r.norm(); return r;
}
inline BigInt operator-(const BigInt& a, const BigInt& b) { BigInt nb = b; if (nb.sign != 0) nb.sign = -nb.sign; return a + nb; }
inline BigInt operator*(const BigInt& a, const BigInt& b) {
    if (a.sign == 0 || b.sign == 0) return BigInt(0);
    BigInt r; r.sign = (a.sign == b.sign) ? 1 : -1; r.d.assign(a.d.size() + b.d.size(), 0);
    for (size_t i = 0; i < a.d.size(); i++) { long long c = 0; for (size_t j = 0; j < b.d.size() || c; j++) { long long cur = r.d[i + j] + c + a.d[i] * (j < b.d.size() ? b.d[j] : 0); r.d[i + j] = cur % BigInt::BASE; c = cur / BigInt::BASE; } }
    r.norm(); return r;
}
inline pair<BigInt, BigInt> big_divmod(const BigInt& a, const BigInt& b) {
    if (a.sign == 0) return {BigInt(0), BigInt(0)};
    if (b.sign == 0) return {BigInt(0), a};
    BigInt A = a.abs(), B = b.abs();
    if (A.cmpAbs(B) < 0) return {BigInt(0), a};
    vector<long long> ad = A.d;
    while (ad.size() > 1 && ad.back() == 0) ad.pop_back();
    int n = (int)ad.size();
    BigInt R(0);
    vector<long long> qdigits;
    for (int i = n - 1; i >= 0; i--) {
        R = R * BigInt(BigInt::BASE) + BigInt(ad[i]);
        long long q = 0;
        if (R.cmpAbs(B) >= 0) {
            long long lo = 0, hi = BigInt::BASE - 1;
            while (lo < hi) {
                long long mid = lo + (hi - lo + 1) / 2;
                if ((B * BigInt(mid)).cmpAbs(R) <= 0) lo = mid; else hi = mid - 1;
            }
            q = lo;
        }
        R = R - B * BigInt(q);
        qdigits.push_back(q);
    }
    std::reverse(qdigits.begin(), qdigits.end());
    BigInt q; q.d = qdigits;
    bool qzero = true; for (auto v : q.d) if (v) { qzero = false; break; }
    q.sign = qzero ? 0 : ((a.sign == b.sign) ? 1 : -1); q.norm();
    bool rzero = true; for (auto v : R.d) if (v) { rzero = false; break; }
    R.sign = rzero ? 0 : a.sign; R.norm();
    return {q, R};
}
inline BigInt operator/(const BigInt& a, const BigInt& b) { return big_divmod(a, b).first; }
inline BigInt operator%(const BigInt& a, const BigInt& b) { return big_divmod(a, b).second; }
inline BigInt big_gcd(const BigInt& a, const BigInt& b) { BigInt x = a.abs(), y = b.abs(); while (y.sign != 0) { BigInt t = x % y; x = y; y = t; } return x; }
inline string toStr(const BigInt& a) { if (a.sign == 0) return "0"; string head = std::to_string(a.d.back()); for (int i = (int)a.d.size() - 2; i >= 0; i--) { string p = std::to_string(a.d[i]); while ((int)p.size() < BigInt::WIDTH) p = "0" + p; head += p; } return (a.sign < 0 ? "-" : "") + head; }
inline BigInt fromStr(const string& s) { BigInt r; r.d.clear(); int i = 0; bool neg = false; if (!s.empty() && s[0] == '-') { neg = true; i = 1; } string num = s.substr(i); if (num.empty()) num = "0"; int n = (int)num.size(); for (int j = n; j > 0; j -= BigInt::WIDTH) { int lo = std::max(0, j - BigInt::WIDTH); r.d.push_back(std::stoll(num.substr(lo, j - lo))); } std::reverse(r.d.begin(), r.d.end()); bool isZero = true; for (auto v : r.d) if (v != 0) { isZero = false; break; } r.sign = isZero ? 0 : (neg ? -1 : 1); r.norm(); return r; }

// ===================== 有理数 Rat（用于精确插值） =====================
struct Rat { BigInt n, d; };
inline Rat makeRat(BigInt a, BigInt b) { if (b.sign < 0) { a = BigInt(0) - a; b = BigInt(0) - b; } BigInt g = big_gcd(a.abs(), b.abs()); if (g.sign == 0) g = BigInt(1); return { a / g, b / g }; }
inline Rat addR(Rat a, Rat b) { return makeRat(a.n * b.d + b.n * a.d, a.d * b.d); }
inline Rat mulR(Rat a, Rat b) { return makeRat(a.n * b.n, a.d * b.d); }

// ===================== 一元多项式 UV (变量 x) =====================
struct UV {
    vector<BigInt> c;
    int deg() const { int d = (int)c.size() - 1; while (d > 0 && c[d] == BigInt(0)) d--; return d; }
    BigInt eval(const BigInt& x) const { BigInt r(0), xp(1); for (size_t i = 0; i < c.size(); i++) { r = r + c[i] * xp; xp = xp * x; } return r; }
    void norm() { while (c.size() > 1 && c.back() == BigInt(0)) c.pop_back(); if (c.empty()) c = {BigInt(0)}; }
    BigInt content() const { BigInt g(0); for (auto& v : c) { g = big_gcd(g, v.abs()); if (g == BigInt(1)) break; } return g; }
    void makePrimitive() { BigInt g = content(); if (g.sign == 0) g = BigInt(1); for (auto& v : c) v = v / g; norm(); }
};
inline UV operator+(const UV& a, const UV& b) { UV r; size_t n = std::max(a.c.size(), b.c.size()); r.c.assign(n, BigInt(0)); for (size_t i = 0; i < n; i++) r.c[i] = (i < a.c.size() ? a.c[i] : BigInt(0)) + (i < b.c.size() ? b.c[i] : BigInt(0)); r.norm(); return r; }
inline UV operator-(const UV& a, const UV& b) { UV r; size_t n = std::max(a.c.size(), b.c.size()); r.c.assign(n, BigInt(0)); for (size_t i = 0; i < n; i++) r.c[i] = (i < a.c.size() ? a.c[i] : BigInt(0)) - (i < b.c.size() ? b.c[i] : BigInt(0)); r.norm(); return r; }
inline UV operator*(const UV& a, const UV& b) { UV r; r.c.assign(a.c.size() + b.c.size() - 1, BigInt(0)); for (size_t i = 0; i < a.c.size(); i++) for (size_t j = 0; j < b.c.size(); j++) r.c[i + j] = r.c[i + j] + a.c[i] * b.c[j]; r.norm(); return r; }
inline pair<UV, UV> divmodUV(const UV& a, const UV& b) {
    UV q; q.c.assign(std::max(0, (int)a.c.size() - (int)b.c.size() + 1), BigInt(0)); UV r = a;
    int db = b.deg(); if (db < 0) return {UV{{BigInt(0)}}, a};
    BigInt lb = b.c[db]; int da = r.deg();
    while (da >= db && !(da == 0 && db == 0)) {
        if ((r.c[da] % lb) != BigInt(0)) break;  // 首项不能被 lb 整除 → h 不是因式，余数非零
        BigInt coef = r.c[da] / lb;
        q.c[da - db] = coef;
        UV term; term.c.assign(da - db + 1, BigInt(0)); term.c[da - db] = coef;
        r = r - term * b; da = r.deg();
    }
    q.norm(); r.norm(); return {q, r};
}
inline vector<BigInt> divisors(const BigInt& n) {
    if (n.sign == 0) return {BigInt(1)};
    string ms = toStr(n.abs()); if (ms.size() > 12) return {BigInt(1)};  // 太大不分解，避免试除卡死
    long long mm = std::stoll(ms); vector<pair<long long, int>> pf; long long t = mm;
    for (long long p = 2; p <= 100000 && p * p <= t; p++) { int e = 0; while (t % p == 0) { t /= p; e++; } if (e) pf.push_back({p, e}); }
    if (t > 1) pf.push_back({t, 1});
    vector<BigInt> res; res.push_back(BigInt(1));
    for (auto& pr : pf) { vector<BigInt> add; for (auto& d : res) { BigInt cur = d; for (int e = 0; e <= pr.second; e++) { if (e) cur = cur * BigInt(pr.first); add.push_back(cur); } } res = add; }
    return res;
}
// 精确拉格朗日插值：给定点 xs, ys(BigInt)，返回次数<=dd 的多项式（Rat 系数）
inline vector<Rat> lagrangeRat(const vector<int>& xs, const vector<BigInt>& ys) {
    int dd = (int)xs.size() - 1;
    vector<Rat> res(dd + 1, makeRat(BigInt(0), BigInt(1)));
    for (int i = 0; i <= dd; i++) {
        vector<Rat> num = { makeRat(BigInt(1), BigInt(1)) };
        BigInt den = BigInt(1);
        for (int j = 0; j <= dd; j++) if (j != i) {
            vector<Rat> t = { makeRat(BigInt(-xs[j]), BigInt(1)), makeRat(BigInt(1), BigInt(1)) };
            vector<Rat> n2(num.size() + 1, makeRat(BigInt(0), BigInt(1)));
            for (size_t a = 0; a < num.size(); a++) for (size_t b = 0; b < t.size(); b++) n2[a + b] = addR(n2[a + b], mulR(num[a], t[b]));
            num = n2; den = den * BigInt(xs[i] - xs[j]);
        }
        for (size_t k = 0; k < num.size(); k++) {
            Rat term = makeRat(ys[i] * num[k].n, num[k].d * den);
            res[k] = addR(res[k], term);
        }
    }
    return res;
}

// Kronecker：找 f(primitive) 的一个真因式；返回空 UV（deg<1）表示不可约
inline UV kroneckerOne(const UV& f) {
    if (gAbort) return UV{{BigInt(0)}};
    int n = f.deg(); if (n <= 1) return UV{{BigInt(0)}};
    int m = n / 2;
    // 选取 m+1 个“非根”整数点作为插值点（根点会让 f(t)=0，divisors(0) 退化）
    vector<int> pts; int t = 0;
    while ((int)pts.size() < m + 1) { if (f.eval(BigInt(t)) != BigInt(0)) pts.push_back(t); if (++t > 200) break; }
    if ((int)pts.size() < m + 1) return UV{{BigInt(0)}};
    vector<BigInt> vals; for (int p : pts) vals.push_back(f.eval(BigInt(p)));
    BigInt lc = f.c[n].abs();
    vector<BigInt> Ldivs = divisors(lc), v0divs = divisors(vals[0]);
    for (int dd = 1; dd <= m; dd++) {
        for (BigInt L : Ldivs) for (BigInt d0base : v0divs) {
            for (int sgn0 : {1, -1}) {
                BigInt d0 = d0base * BigInt(sgn0);
                vector<BigInt> chosen(dd + 1); chosen[0] = d0;
                bool found = false; UV result;
                function<void(int)> bt = [&](int tk) {
                    if (found || gAbort) return;
                    if (++gWork > WORK_LIMIT || overTime()) { gAbort = true; return; }
                    if (tk > dd) {
                        vector<int> subPts(pts.begin(), pts.begin() + dd + 1);
                        vector<Rat> hr = lagrangeRat(subPts, chosen);
                        UV h; h.c.resize(hr.size(), BigInt(0)); bool allInt = true;
                        for (size_t k = 0; k < hr.size(); k++) { if (hr[k].d != BigInt(1)) { allInt = false; break; } h.c[k] = hr[k].n; }
                        if (!allInt) return;
                        h.norm(); int hd = h.deg();
                        if (hd < 1 || hd > n - 1) return;
                        if (!(h.c[hd] == L) && !(h.c[hd] == (BigInt(0) - L))) return;
                        auto qr = divmodUV(f, h); UV& q = qr.first; UV& r = qr.second;
                        (void)q;
                        for (auto& v : r.c) if (v != BigInt(0)) return;
                        if (h.c[hd].sign < 0) for (auto& v : h.c) v = BigInt(0) - v;
                        result = h; found = true; return;
                    }
                    for (BigInt d : divisors(vals[tk])) for (int sgn : {1, -1}) { if (gAbort || found) return; BigInt v = d * BigInt(sgn); chosen[tk] = v; bt(tk + 1); }
                };
                bt(1);
                if (found) return result;
                if (gAbort) return UV{{BigInt(0)}};
            }
        }
    }
    return UV{{BigInt(0)}};
}
inline vector<UV> factorUV(UV f) {
    f.makePrimitive();
    if (f.c[f.deg()].sign < 0) for (auto& v : f.c) v = BigInt(0) - v;  // 统一符号，便于比较因式
    vector<UV> res;
    int guard = 0;
    while (f.deg() >= 1) {
        if (++guard > 500) break;  // 防跑飞
        UV g = kroneckerOne(f);
        if (g.deg() < 1) { res.push_back(f); break; }
        while (true) { auto qr = divmodUV(f, g); UV& q = qr.first; UV& r = qr.second; bool z = true; for (auto& v : r.c) if (v != BigInt(0)) { z = false; break; } if (!z) break; res.push_back(g); f = q; }
        if (f.deg() < 1) break;
    }
    return res;
}

// ===================== 多元多项式 MPoly =====================
struct MPoly {
    int nvars = 0; map<vector<int>, BigInt> t;
    MPoly() {} MPoly(int n) : nvars(n < 0 ? 0 : n) {}
    static int expAt(const vector<int>& e, int i) { return i < (int)e.size() ? e[i] : 0; }
    void addTerm(const vector<int>& e, BigInt c) {
        if (c == BigInt(0)) return;
        vector<int> ee = e;
        while ((int)ee.size() < nvars) ee.push_back(0);
        auto it = t.find(ee); if (it == t.end()) t[ee] = c; else { it->second = it->second + c; if (it->second == BigInt(0)) t.erase(it); }
    }
    MPoly operator*(const MPoly& o) const { int N = std::max(nvars, o.nvars); MPoly r(N); for (auto& a : t) for (auto& b : o.t) { vector<int> e(N); for (int i = 0; i < N; i++) e[i] = expAt(a.first, i) + expAt(b.first, i); r.addTerm(e, a.second * b.second); } return r; }
    MPoly operator+(const MPoly& o) const { int N = std::max(nvars, o.nvars); MPoly r(N); for (auto& a : t) r.addTerm(a.first, a.second); for (auto& b : o.t) r.addTerm(b.first, b.second); return r; }
    MPoly operator-(const MPoly& o) const { int N = std::max(nvars, o.nvars); MPoly r(N); for (auto& a : t) r.addTerm(a.first, a.second); for (auto& b : o.t) r.addTerm(b.first, BigInt(0) - b.second); return r; }
    MPoly operator*(const BigInt& v) const { MPoly r(nvars); for (auto& a : t) r.addTerm(a.first, a.second * v); return r; }
    void norm() { for (auto it = t.begin(); it != t.end(); ) { if (it->second == BigInt(0)) it = t.erase(it); else ++it; } }
    int degIn(int v) const { int m = 0; for (auto& a : t) m = std::max(m, expAt(a.first, v)); return m; }
    BigInt integerContent() const { BigInt g(0); for (auto& a : t) { g = big_gcd(g, a.second.abs()); if (g == BigInt(1)) break; } return g; }
    MPoly primitive() const { BigInt g = integerContent(); if (g.sign == 0) g = BigInt(1); MPoly r(nvars); for (auto& a : t) r.addTerm(a.first, a.second / g); return r; }
    MPoly evalVar(int v, const BigInt& val) const { MPoly r(nvars); for (auto& a : t) { BigInt coeff = a.second; int ev = expAt(a.first, v); if (ev > 0) { BigInt f = val; for (int k = 1; k < ev; k++) f = f * val; coeff = coeff * f; } vector<int> e = a.first; while ((int)e.size() < nvars) e.push_back(0); e[v] = 0; r.addTerm(e, coeff); } return r; }
    UV toUV0() const { UV r; int d = degIn(0); r.c.assign(d + 1, BigInt(0)); for (auto& a : t) r.c[expAt(a.first, 0)] = r.c[expAt(a.first, 0)] + a.second; r.norm(); return r; }
    bool operator==(const MPoly& o) const { MPoly d = (*this) - o; d.norm(); return d.t.empty(); }
};

inline MPoly encodeToBivar(const MPoly& f, int M, int& Ydeg) {
    MPoly g(2); Ydeg = 0;
    for (auto& a : f.t) { int E = 0, p = 1; for (int i = 1; i < f.nvars; i++) { E += a.first[i] * p; p *= M; } Ydeg = std::max(Ydeg, E); vector<int> e(2); e[0] = a.first[0]; e[1] = E; g.addTerm(e, a.second); }
    return g;
}
inline MPoly decodeFromBivar(const MPoly& g, int nvars, int M) {
    MPoly f(nvars);
    for (auto& a : g.t) { int E = a.first[1]; vector<int> e(nvars, 0); e[0] = a.first[0]; int idx = 1; while (E > 0) { e[idx] = E % M; E /= M; idx++; } f.addTerm(e, a.second); }
    return f;
}

// 取 F 中 x^xi 的系数（关于 Y 的一元多项式）
inline UV coeffX(const MPoly& F, int xi) {
    UV r; r.c.assign(F.degIn(1) + 1, BigInt(0));
    for (auto& a : F.t) if (MPoly::expAt(a.first, 0) == xi) {
        int e = MPoly::expAt(a.first, 1);
        r.c[e] = r.c[e] + a.second;
    }
    r.norm(); return r;
}
inline bool isZeroUV(const UV& u) { for (auto& v : u.c) if (v != BigInt(0)) return false; return true; }

// 二元因式分解（变量 0 = x, 1 = Y）
inline vector<MPoly> factorBivar(MPoly F) {
    F = F.primitive();
    int dx = F.degIn(0), dy = F.degIn(1);
    if (dy == 0) { // 不含 Y，退化为一元（变量 0）
        vector<MPoly> res; for (auto& g : factorUV(F.toUV0())) { MPoly gg(2); for (size_t i = 0; i < g.c.size(); i++) if (g.c[i] != BigInt(0)) { vector<int> e(2); e[0] = (int)i; gg.addTerm(e, g.c[i]); } res.push_back(gg); } return res;
    }
    if (dx == 0) { // 只含 Y
        MPoly f1(1); for (auto& a : F.t) { vector<int> e(1); e[0] = a.first[1]; f1.addTerm(e, a.second); }
        vector<MPoly> res; for (auto& g : factorUV(f1.toUV0())) { MPoly gg(2); for (size_t i = 0; i < g.c.size(); i++) if (g.c[i] != BigInt(0)) { vector<int> e(2); e[1] = (int)i; gg.addTerm(e, g.c[i]); } res.push_back(gg); } return res;
    }
    // 提取关于主变量 x 的 content：各 x 次系数的公共一元因式（只含 Y）
    // 复用 factorUV 的不可约因式列表求多重集交，避免另写 Q[Y] 上的多项式 gcd
    {
        vector<UV> common; bool init = false;
        for (int xi = 0; xi <= dx; xi++) {
            UV cf = coeffX(F, xi); if (isZeroUV(cf)) continue;   // 零系数不约束 gcd
            vector<UV> fi = factorUV(cf);
            if (!init) { common = fi; init = true; }
            else {
                vector<UV> nc; vector<bool> used(fi.size(), false);
                for (auto& g : common) for (size_t j = 0; j < fi.size(); j++) if (!used[j] && fi[j].c.size() == g.c.size()) {
                    bool eq = true; for (size_t q = 0; q < g.c.size(); q++) if (fi[j].c[q] != g.c[q]) { eq = false; break; }
                    if (eq) { used[j] = true; nc.push_back(g); break; }
                }
                common = nc;
            }
        }
        if (!common.empty()) {
            vector<MPoly> res;
            for (auto& g : common) {
                MPoly gg(2); for (size_t i = 0; i < g.c.size(); i++) if (g.c[i] != BigInt(0)) { vector<int> e(2, 0); e[1] = (int)i; gg.addTerm(e, g.c[i]); }
                res.push_back(gg);
            }
            MPoly F1 = F; bool okDiv = true;
            for (auto& g : common) {
                MPoly Q(2);
                for (int xi = 0; xi <= F1.degIn(0); xi++) {
                    UV a = coeffX(F1, xi); if (isZeroUV(a)) continue;
                    auto qr = divmodUV(a, g); UV& q = qr.first; UV& r = qr.second;
                    if (!isZeroUV(r)) { okDiv = false; break; }
                    for (size_t j = 0; j < q.c.size(); j++) if (q.c[j] != BigInt(0)) { vector<int> e(2, 0); e[0] = xi; e[1] = (int)j; Q.addTerm(e, q.c[j]); }
                }
                if (!okDiv) break;
                F1 = Q;
            }
            if (okDiv) {
                F1 = F1.primitive();
                vector<MPoly> rest = factorBivar(F1);
                res.insert(res.end(), rest.begin(), rest.end());
                return res;
            }
        }
    }
    // t0=0 常使 F(x,0) 出现重因式/退化，优先级放最后，先用一般点
    for (int ti = 0; ti <= 12 && !gAbort; ti++) {
        int t0 = (ti == 12) ? 0 : ti + 1;
        MPoly Ft0 = F.evalVar(1, BigInt(t0));
        if (Ft0.toUV0().deg() != dx) continue;
        vector<UV> Glist = factorUV(Ft0.toUV0());
        // 允许 F(x,t0) 有重因式（完全平方等）：重复的 G 会各自恢复出同一个 h，
        // 最后由 prod == F 精确校验兜底，不再跳过这类点。
        // 插值点数封顶：真因式关于 Y 的次数通常远小于编码后的 dy，
        // 点数少可把组合枚举从 2^dy 降到 2^5；不足时由 prod == F 精确校验兜底。
        // 各 Y 点的一元分解对所有 G 相同，提到 G 循环外只算一次。
        int need = std::min(dy + 1, 6);
        vector<int> ypts; for (int k = 0; k < need; k++) ypts.push_back(t0 + k);
        vector<vector<UV>> L(ypts.size()); bool badAll = false;
        for (size_t k = 0; k < ypts.size(); k++) {
            if (gAbort) { badAll = true; break; }
            MPoly Fk = F.evalVar(1, BigInt(ypts[k]));
            if (Fk.toUV0().deg() != dx) { badAll = true; break; }
            L[k] = factorUV(Fk.toUV0());
        }
        if (badAll) continue;
        vector<MPoly> factors; bool ok = true;
        for (auto& G : Glist) {
            int dd = G.deg();
            vector<UV> chosen(ypts.size()); chosen[0] = G;
            MPoly best(2); bool found = false;
            function<void(int, vector<BigInt>&)> rec = [&](int k, vector<BigInt>& lcPts) {
                if (found || gAbort) return;
                if (++gWork > WORK_LIMIT || overTime()) { gAbort = true; return; }
                if (k == (int)ypts.size()) {
                    MPoly h(2);
                    for (int i = 0; i <= dd; i++) {
                        vector<BigInt> ys; for (int kk = 0; kk < (int)ypts.size(); kk++) ys.push_back(i < (int)chosen[kk].c.size() ? chosen[kk].c[i] : BigInt(0));
                        vector<Rat> polyR = lagrangeRat(ypts, ys);
                        for (size_t j = 0; j < polyR.size(); j++) { if (polyR[j].d != BigInt(1)) return; if (polyR[j].n != BigInt(0)) { vector<int> e(2); e[0] = i; e[1] = (int)j; h.addTerm(e, polyR[j].n); } }
                    }
                    h = h.primitive();  // 去掉整数常数倍，否则数值整除检验会误判
                    MPoly ht0 = h.evalVar(1, BigInt(t0)); UV hu = ht0.toUV0();
                    // 插值恢复的因式允许与 G 相差任意非零常数倍（各 Y 点的一元因式可独立带符号/常数）
                    bool prop = (hu.c.size() == G.c.size());
                    if (prop) { bool nz = false; for (auto& v : hu.c) if (v != BigInt(0)) nz = true; if (!nz) prop = false; }
                    if (prop) for (size_t p = 0; p < hu.c.size() && prop; p++) for (size_t q = 0; q < hu.c.size() && prop; q++)
                        if (G.c[p] * hu.c[q] != G.c[q] * hu.c[p]) prop = false;
                    if (!prop) return;
                    bool div = true;
                    for (int a = -3; a <= 3 && div; a++) for (int b = -3; b <= 3 && div; b++) {
                        BigInt Fa(0), ha(0);
                        for (auto& term : F.t) { BigInt v = term.second; BigInt xa = BigInt(a), xb = BigInt(b); for (int kk = 0; kk < term.first[0]; kk++) v = v * xa; for (int kk = 0; kk < term.first[1]; kk++) v = v * xb; Fa = Fa + v; }
                        for (auto& term : h.t) { BigInt v = term.second; BigInt xa = BigInt(a), xb = BigInt(b); for (int kk = 0; kk < term.first[0]; kk++) v = v * xa; for (int kk = 0; kk < term.first[1]; kk++) v = v * xb; ha = ha + v; }
                        if (ha != BigInt(0)) {
                            BigInt r = Fa % ha;
                            if (r != BigInt(0)) div = false;
                        }
                    }
                    if (div) { best = h; found = true; }
                    return;
                }
                for (auto& cand : L[k]) {
                    if (gAbort || found) return;
                    if (cand.deg() != dd) continue;
                    chosen[k] = cand; lcPts.push_back(cand.c[dd]);
                    rec(k + 1, lcPts); lcPts.pop_back();
                    if (found) return;
                }
            };
            vector<BigInt> lc0; lc0.push_back(G.c[dd]);
            rec(1, lc0);
            if (!found) { ok = false; break; }
            best = best.primitive();
            int hd = 0; for (auto& tt : best.t) hd = std::max(hd, tt.first[0]);
            bool neg = false; for (auto& tt : best.t) if (tt.first[0] == hd && tt.second.sign < 0) neg = true;
            if (neg) { MPoly nb(2); for (auto& tt : best.t) nb.addTerm(tt.first, BigInt(0) - tt.second); best = nb; }
            factors.push_back(best);
        }
        if (!ok) continue;
        MPoly prod(2); { vector<int> e(2, 0); prod.addTerm(e, BigInt(1)); }
        for (auto& h : factors) prod = prod * h;
        if (prod == F) return factors;
        // 因式的符号规范化只保证到整体 ±1，尝试把 -1 吸收进第一个因式
        MPoly nprod(2); for (auto& tt : prod.t) nprod.addTerm(tt.first, BigInt(0) - tt.second);
        if (!factors.empty() && nprod == F) {
            MPoly nb(2); for (auto& tt : factors[0].t) nb.addTerm(tt.first, BigInt(0) - tt.second);
            factors[0] = nb; return factors;
        }
    }
    return {F};
}

// ===================== 解析器 =====================
struct Parser {
    string s; size_t i = 0; map<char, int> varIndex; int nvars = 0;
    int varId(char c) { if (!varIndex.count(c)) varIndex[c] = nvars++; return varIndex[c]; }
    char peek() { while (i < s.size() && s[i] == ' ') i++; return i < s.size() ? s[i] : 0; }
    char get() { while (i < s.size() && s[i] == ' ') i++; return i < s.size() ? s[i++] : 0; }
    MPoly parse() { return expr(); }
    MPoly expr() { MPoly a = term(); while (true) { char c = peek(); if (c == '+') { get(); a = a + term(); } else if (c == '-') { get(); a = a - term(); } else break; } return a; }
    MPoly term() { MPoly a = factor(); while (true) { char c = peek(); if (c == '*') { get(); a = a * factor(); } else if (c == '(' || std::isalpha(c) || std::isdigit(c)) { a = a * factor(); } else break; } return a; }
    MPoly factor() {
        char c = peek();
        if (c == '-') { get(); MPoly a = factor(); return a * BigInt(-1); }  // 一元负号：-(...)
        if (c == '(') { get(); MPoly a = expr(); get(); MPoly base = a; if (peek() == '^') { get(); BigInt e = number(); long long pw = std::atoll(toStr(e).c_str()); MPoly res(nvars); { vector<int> z(nvars, 0); res.addTerm(z, BigInt(1)); } for (long long k = 0; k < pw; k++) res = res * base; return res; } return base; }
        if (std::isalpha(c)) { get(); int id = varId(c); MPoly r(nvars); vector<int> e(nvars, 0); e[id] = 1; r.addTerm(e, BigInt(1)); if (peek() == '^') { get(); BigInt e2 = number(); long long pw = std::atoll(toStr(e2).c_str()); MPoly base = r; MPoly res(nvars); { vector<int> z(nvars, 0); res.addTerm(z, BigInt(1)); } for (long long k = 0; k < pw; k++) res = res * base; return res; } return r; }
        BigInt v = number(); MPoly r(nvars); vector<int> e(nvars, 0); r.addTerm(e, v); return r;
    }
    BigInt number() { string num; char c = peek(); if (c == '-') { num += '-'; get(); c = peek(); } while (c != 0 && std::isdigit((unsigned char)c)) { num += c; get(); c = peek(); } return fromStr(num.empty() ? "0" : num); }
};
inline string varName(int id) { return string(1, gVarNames.empty() ? (char)('a' + id) : gVarNames[id]); }
inline string polyToStr(const MPoly& f) {
    if (f.t.empty()) return "0";
    vector<pair<vector<int>, BigInt>> terms(f.t.begin(), f.t.end());
    typedef std::pair<std::vector<int>, BigInt> Term;
    std::sort(terms.begin(), terms.end(), [](const Term& A, const Term& B) { int sa = 0, sb = 0; for (int x : A.first) sa += x; for (int x : B.first) sb += x; if (sa != sb) return sa > sb; return A.first > B.first; });
    string res;
    for (size_t idx = 0; idx < terms.size(); idx++) {
        auto& e = terms[idx].first; auto& c = terms[idx].second;
        string ts; for (int v = 0; v < f.nvars; v++) { if (e[v] == 0) continue; string vs = varName(v); if (e[v] == 1) ts += vs; else ts += vs + "^" + std::to_string(e[v]); }
        if (ts.empty()) ts = "1";
        string cs = toStr(c.abs());
        if (ts != "1" && cs == "1") cs = "";
        string part = (idx == 0 ? (c.sign < 0 ? "-" : "") : (c.sign < 0 ? " - " : " + "));
        if (cs.empty()) part += ts; else part += cs + (ts == "1" ? "" : "*" + ts);
        res += part;
    }
    return res;
}
// 把二元因式 g(x,Y) 解码回多元：Y^E 按 base-M 还原各变量指数。
// 若某位超过原多项式的最高指数，说明这是编码引入的“伪因式”，单独解码非法。
inline bool decodeLegal(const MPoly& g, int nvars, int M, int maxExp, MPoly& out) {
    MPoly f(nvars);
    for (auto& a : g.t) {
        int E = MPoly::expAt(a.first, 1), rest = E, idx = 1;
        vector<int> e(nvars, 0); e[0] = MPoly::expAt(a.first, 0);
        while (rest > 0) {
            if (idx >= nvars || rest % M > maxExp) return false;
            e[idx] = rest % M; rest /= M; idx++;
        }
        f.addTerm(e, a.second);
    }
    out = f; return true;
}
// 二元因式可能需要重新分组才能合法解码（例如 (Y-1)(Y²+Y+1)=Y³-1 才对应 z-1）。
// 用子集 DP 找组数最多的划分，并用精确乘积校验兜底；失败则保守退回原多项式。
inline vector<MPoly> decodeGroups(const vector<MPoly>& bf, int nvars, int M, int maxExp, const MPoly& Fp) {
    int k = (int)bf.size(); if (k == 0 || k > 20) return {Fp};
    int full = 1 << k;
    vector<MPoly> gPoly(full), gDec(full);
    vector<char> legal(full, 0);
    for (int mask = 1; mask < full; mask++) {
        int low = mask & -mask, id = 0; while (!((low >> id) & 1)) ++id;
        gPoly[mask] = (mask == low) ? bf[id] : gPoly[mask ^ low] * bf[id];
        legal[mask] = decodeLegal(gPoly[mask], nvars, M, maxExp, gDec[mask]) ? 1 : 0;
    }
    vector<int> cnt(full, -1), choice(full, -1); cnt[0] = 0;
    for (int mask = 1; mask < full; mask++) {
        int low = mask & -mask;
        for (int sub = mask; sub; sub = (sub - 1) & mask) if ((sub & low) && legal[sub]) {
            int rest = mask ^ sub;
            if (cnt[rest] >= 0 && cnt[rest] + 1 > cnt[mask]) { cnt[mask] = cnt[rest] + 1; choice[mask] = sub; }
        }
    }
    if (cnt[full - 1] < 0) return {Fp};
    vector<MPoly> out; int m = full - 1;
    while (m) { out.push_back(gDec[choice[m]]); m ^= choice[m]; }
    MPoly prod(nvars); { vector<int> z(nvars, 0); prod.addTerm(z, BigInt(1)); }
    for (auto& g : out) prod = prod * g;
    if (prod == Fp) return out;
    MPoly nprod(nvars); for (auto& a : prod.t) nprod.addTerm(a.first, BigInt(0) - a.second);
    if (nprod == Fp) { MPoly nb(nvars); for (auto& a : out[0].t) nb.addTerm(a.first, BigInt(0) - a.second); out[0] = nb; return out; }
    return {Fp};
}

} // namespace poly_factor_detail

struct PolyFactorResult {
    std::string constant;                    // 整数 content，"1" 表示无
    std::vector<std::string> factors;        // 各不可约因式的可读字符串
    std::string expression() const {         // 完整分解式
        if (factors.empty()) return constant;
        std::string s;
        if (constant == "-1") s = "-";
        else if (constant != "1") s = constant + " * ";
        for (size_t i = 0; i < factors.size(); i++) {
            const std::string& t = factors[i];
            bool needP = (t.find('+') != std::string::npos) || (t.find('-', 1) != std::string::npos);
            s += (i ? " * " : "") + (needP ? "(" + t + ")" : t);
        }
        return s;
    }
};

// 对一条表达式做完全因式分解
inline PolyFactorResult factor_polynomial(const std::string& expr) {
    using namespace poly_factor_detail;
    gWork = 0; gAbort = false; gNVars = 0; gStart = clock();
    PolyFactorResult R; R.constant = "1";
    std::string line = expr;
    size_t h = line.find('#'); if (h != std::string::npos) line = line.substr(0, h);
    if (line.empty()) return R;
    Parser p; p.s = line; MPoly f = p.parse(); int nvars = p.nvars;
    if (nvars == 0) { R.constant = toStr(f.integerContent()); return R; }
    gVarNames.assign(nvars, '?'); for (auto& kv : p.varIndex) gVarNames[kv.second] = kv.first;
    BigInt C = f.integerContent(); MPoly Fp = f.primitive();
    std::vector<MPoly> varFactors;
    for (int v = 0; v < nvars; v++) {
        int mn = INT_MAX;
        for (auto& a : Fp.t) mn = std::min(mn, MPoly::expAt(a.first, v));
        if (mn == INT_MAX || mn <= 0) continue;
        for (int k = 0; k < mn; k++) { MPoly one(nvars); std::vector<int> z(nvars, 0); z[v] = 1; one.addTerm(z, BigInt(1)); varFactors.push_back(one); }
        MPoly q(nvars);
        for (auto& a : Fp.t) { std::vector<int> ne(nvars, 0); for (int w = 0; w < nvars; w++) ne[w] = MPoly::expAt(a.first, w); ne[v] -= mn; q.addTerm(ne, a.second); }
        Fp = q;
    }
    R.constant = toStr(C);
    bool onlyConst = true;
    for (auto& a : Fp.t) for (int v = 0; v < nvars; v++) if (MPoly::expAt(a.first, v) != 0) onlyConst = false;
    if (onlyConst) { for (auto& g : varFactors) R.factors.push_back(polyToStr(g)); return R; }
    int maxExp = 0; for (auto& a : Fp.t) for (int v = 1; v < nvars; v++) maxExp = std::max(maxExp, MPoly::expAt(a.first, v));
    int M = maxExp + 2, Ydeg;
    MPoly biv = encodeToBivar(Fp, M, Ydeg);
    std::vector<MPoly> bf = factorBivar(biv);
    std::vector<MPoly> nf = decodeGroups(bf, nvars, M, maxExp, Fp);
    for (auto& g : varFactors) nf.push_back(g);
    for (auto& g : nf) R.factors.push_back(polyToStr(g));
    return R;
}
inline std::string factorize(const std::string& expr) { return factor_polynomial(expr).expression(); }

} // namespace better_std

#endif // BETTER_STD_STD_POLY_FACTOR_HPP
