// 跨模块回归测试：每个断言都对比已知值或暴力结果。
// 编译：g++ -std=c++11 -pedantic-errors -O2 -static -o test_all test_all.cpp
// 退出码 0 = 全过；非 0 = 失败数。
#include"../std_better.hpp"
#include<cstdio>
#include<cstdlib>
#include<vector>
#include<string>
#include<sstream>
#include<algorithm>
#include<random>
using namespace better_std;

static int failures = 0;
static long long checks = 0;
#define CHECK(cond) do { ++checks; if(!(cond)){ \
    printf("FAIL line %d: %s\n", __LINE__, #cond); ++failures; } } while(0)

static std::mt19937_64 rng(20261120);
static long long rll(long long lo, long long hi) {
    return lo + (long long)(rng() % (unsigned long long)(hi - lo + 1));
}

//—— 数值 / 高精度 ——
static void test_numeric() {
    High_precision_number<30> a("12345678901234567890"), b(7);
    std::stringstream ss; ss << (a * b + b);
    CHECK(ss.str() == "86419752308641975237");
    CHECK(modpow(2, 10, 1000000007) == 1024);
    CHECK(first_true([](long long x){ return x * x >= 50; }, 0, 20) == 8);
}

//—— 排序 / 数据结构 ——
static void test_structures() {
    std::vector<int> v = {5, 3, 9, 1, 7};
    quick_sort(v.begin(), v.end());
    CHECK((v == std::vector<int>{1, 3, 5, 7, 9}));

    std_fenwick<long long> fw(10);
    fw.add(3, 5); fw.add(6, 2);
    CHECK(fw.sum(5) == 5);
    CHECK(fw.range_sum(4, 6) == 2);

    rollback_dsu rd(4);
    rd.unite(0, 1); rd.unite(2, 3);
    CHECK(rd.same(0, 1) && rd.same(2, 3));
    rd.undo();
    CHECK(!rd.same(2, 3) && rd.same(0, 1));
}

//—— 字符串 ——
static void test_strings() {
    suffix_array sa("banana");
    CHECK(sa.sa.size() == 6);
    CHECK(sa.sa[0] == 5);                 // 最小后缀 "a"
    rolling_hash rh("abracadabra");
    CHECK(rh.equal(0, 7, 2));             // "ab" == "ab"
    CHECK(!rh.equal(0, 3, 2));
}

//—— 图 / 网络流 ——
static void test_graph() {
    std_dinic<long long> dinic(4);
    dinic.add_edge(0, 1, 3); dinic.add_edge(0, 2, 2);
    dinic.add_edge(1, 3, 2); dinic.add_edge(2, 3, 3);
    CHECK(dinic.max_flow(0, 3) == 4);

    std::vector<std::vector<int>> g = {{1}, {0, 2}, {1}};
    auto tr = tarjan(g);
    CHECK(tr.is_cut[1] != 0);
    CHECK(tr.bridges.size() == 2);

    two_sat sat;
    sat.reset(2);
    sat.add(0, true, 1, true);
    sat.add(0, false, 1, true);
    sat.add(0, true, 1, false);
    std::vector<char> asg;
    CHECK(sat.satisfiable(&asg));
}

//—— 树 ——
static void test_trees() {
    std::vector<std::vector<int>> g = {{1, 3}, {0, 2}, {1}, {0}};
    tree_lca lca((int)g.size(), g);
    CHECK(lca.lca(2, 3) == 0);
    CHECK(lca.dist(2, 3) == 3);

    link_cut_tree<long long> lct(4);
    for (int i = 0; i < 4; i++) lct.set_val(i, i + 1);
    lct.link(0, 1); lct.link(1, 2); lct.link(2, 3);
    CHECK(lct.connected(0, 3));
    CHECK(lct.path_max(0, 3) == 4);
    lct.path_add(1, 2, 10);
    CHECK(lct.path_max(0, 3) == 13);
    lct.cut(1, 2);
    CHECK(!lct.connected(0, 3));
}

//—— 数论 / 多项式 ——
static void test_number_theory() {
    auto r = crt_general({2, 3}, {3, 5});
    CHECK(r.first && r.second.first == 8 && r.second.second == 15);

    CHECK(prime_factors(600851475143ULL).size() == 4);
    CHECK(divisor_count(600851475143ULL) == 16);

    CHECK(factorize("x^2-1").find("x - 1") != std::string::npos);

    std::vector<int> fib = {0, 1};
    for (int i = 2; i < 30; i++) fib.push_back(fib[i-1] + fib[i-2]);
    auto rec = berlekamp_massey(fib);
    CHECK(rec.size() == 2);
    CHECK(linear_rec_nth(fib, rec, 30) == 832040);
}

//—— 随机对拍：3SUM 计数 vs 暴力 ——
static void test_three_sum_random() {
    for (int t = 0; t < 60; t++) {
        int n = (int)rll(3, 12);
        std::vector<int> a(n);
        for (int i = 0; i < n; i++) a[i] = (int)rll(-6, 6);
        int target = (int)rll(-8, 8);
        long long got = three_sum_count(a, target), want = 0;
        for (int i = 0; i < n; i++)
            for (int j = i+1; j < n; j++)
                for (int k = j+1; k < n; k++)
                    if ((long long)a[i]+a[j]+a[k] == target) want++;
        CHECK(got == want);
    }
}

//—— 随机对拍：小波树第 k 小 vs 排序 ——
static void test_wavelet_random() {
    for (int t = 0; t < 40; t++) {
        int n = (int)rll(1, 50);
        std::vector<int> a(n);
        for (int i = 0; i < n; i++) a[i] = (int)rll(-50, 50);
        wavelet_tree<int> wt(a);
        int l = (int)rll(0, n-1), r = (int)rll(0, n-1);
        if (l > r) std::swap(l, r);
        std::vector<int> seg(a.begin()+l, a.begin()+r+1);
        std::sort(seg.begin(), seg.end());
        for (int k = 0; k < (int)seg.size(); k++)
            CHECK(wt.kth(l, r, k) == seg[k]);   // kth 是 0-based 闭区间 [l,r]
    }
}

//—— 随机对拍：整体二分第 k 小 vs 排序 ——
static void test_overall_binary_random() {
    for (int t = 0; t < 25; t++) {
        int n = (int)rll(1, 20);
        std::vector<int> a(n);
        for (int i = 0; i < n; i++) a[i] = (int)rll(1, 30);
        std::vector<ob_query> qs;
        for (int q = 0; q < 10; q++) {
            int l = (int)rll(0, n-1), r = (int)rll(0, n-1);
            if (l > r) std::swap(l, r);
            int len = r - l + 1, k = (int)rll(1, len);
            ob_query oq; oq.l = l; oq.r = r; oq.k = k; oq.id = (int)qs.size();
            qs.push_back(oq);
        }
        auto ans = overall_binary_range_kth(a, qs, 1, 30);
        for (size_t i = 0; i < qs.size(); i++) {
            std::vector<int> seg(a.begin()+qs[i].l, a.begin()+qs[i].r+1);
            std::sort(seg.begin(), seg.end());
            CHECK(ans[i] == seg[qs[i].k-1]);
        }
    }
}

int main() {
    test_numeric();
    test_structures();
    test_strings();
    test_graph();
    test_trees();
    test_number_theory();
    test_three_sum_random();
    test_wavelet_random();
    test_overall_binary_random();
    printf("RESULT checks=%lld failures=%d %s\n", checks, failures,
           failures ? "FAIL" : "PASS");
    return failures ? 1 : 0;
}
