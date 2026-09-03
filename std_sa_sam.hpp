#pragma once
//===== better_std::std_sa_sam —— 后缀数组 / 后缀自动机 =====
//  suffix_array(s)  : 构造后缀数组 sa / 名次数组 rk / 高度数组 lcp（Kasai），
//                     并提供 lcp_query(i,j) = 后缀 i 与后缀 j 的最长公共前缀（RMQ）。
//  suffix_automaton : 后缀自动机，支持不同子串计数、模式串出现次数、最长子串长度。
#include<bits/stdc++.h>
namespace better_std {

//==================== 后缀数组 ====================
struct suffix_array {
    std::string s;
    int n;
    std::vector<int> sa, rk, lcp, inv;
    std::vector<std::vector<int>> st;   // lcp 的稀疏表（RMQ）
    std::vector<int> log2;

    explicit suffix_array(const std::string& str) : s(str), n((int)str.size()) {
        build();
        build_lcp();
        build_rmq();
    }

    void build() {
        sa.resize(n); rk.resize(n);
        for (int i = 0; i < n; i++) { sa[i] = i; rk[i] = (unsigned char)s[i]; }
        for (int k = 1; k < n; k <<= 1) {
            auto cmp = [&](int a, int b) {
                if (rk[a] != rk[b]) return rk[a] < rk[b];
                int ra = (a + k < n) ? rk[a + k] : -1;
                int rb = (b + k < n) ? rk[b + k] : -1;
                return ra < rb;
            };
            std::sort(sa.begin(), sa.end(), cmp);
            std::vector<int> tmp(n);
            tmp[sa[0]] = 0;
            for (int i = 1; i < n; i++)
                tmp[sa[i]] = tmp[sa[i - 1]] + (cmp(sa[i - 1], sa[i]) ? 1 : 0);
            rk = tmp;
            if (rk[sa[n - 1]] == n - 1) break;   // 已全不同，提前结束
        }
        inv.resize(n);
        for (int i = 0; i < n; i++) inv[sa[i]] = i;
    }

    void build_lcp() {
        lcp.assign(n, 0);
        int h = 0;
        for (int i = 0; i < n; i++) {
            if (inv[i] == 0) { h = 0; continue; }
            int j = sa[inv[i] - 1];
            while (i + h < n && j + h < n && s[i + h] == s[j + h]) h++;
            lcp[inv[i]] = h;
            if (h > 0) h--;
        }
    }

    void build_rmq() {
        log2.assign(n + 1, 0);
        for (int i = 2; i <= n; i++) log2[i] = log2[i / 2] + 1;
        int K = (n ? log2[n] : 0) + 1;
        st.assign(K, std::vector<int>(n, 0));
        for (int i = 0; i < n; i++) st[0][i] = lcp[i];
        for (int k = 1; k < K; k++)
            for (int i = 0; i + (1 << k) <= n; i++)
                st[k][i] = std::min(st[k - 1][i], st[k - 1][i + (1 << (k - 1))]);
    }

    // 后缀 i 与后缀 j（起始位置）的最长公共前缀长度
    int lcp_query(int i, int j) const {
        if (i == j) return n - i;
        int r1 = inv[i], r2 = inv[j];
        if (r1 > r2) std::swap(r1, r2);
        int len = r2 - r1;
        int k = log2[len];
        return std::min(st[k][r1 + 1], st[k][r2 - (1 << k) + 1]);
    }
};

//==================== 后缀自动机 ====================
struct sam_state {
    int len, link;
    std::map<char, int> next;
    sam_state() : len(0), link(-1) {}
};

struct suffix_automaton {
    std::vector<sam_state> st;
    std::vector<long long> occ;   // 每个状态的 endpos 大小（出现次数）
    std::vector<int> term;        // term[i] = 加入第 i 个字符后的 last 状态
    int last, sz;

    suffix_automaton() { reset(); }
    void reset() { st.clear(); st.emplace_back(); last = 0; sz = 1; occ.clear(); term.clear(); }

    void extend(char c) {
        int cur = sz++; st.emplace_back();
        st[cur].len = st[last].len + 1;
        int p = last;
        while (p != -1 && !st[p].next.count(c)) { st[p].next[c] = cur; p = st[p].link; }
        if (p == -1) {
            st[cur].link = 0;
        } else {
            int q = st[p].next[c];
            if (st[p].len + 1 == st[q].len) {
                st[cur].link = q;
            } else {
                int clone = sz++; st.emplace_back();
                st[clone].len = st[p].len + 1;
                st[clone].next = st[q].next;
                st[clone].link = st[q].link;
                while (p != -1 && st[p].next[c] == q) { st[p].next[c] = clone; p = st[p].link; }
                st[q].link = st[cur].link = clone;
            }
        }
        last = cur;
    }

    void build(const std::string& s) {
        reset();
        term.resize(s.size());
        for (int i = 0; i < (int)s.size(); i++) { extend(s[i]); term[i] = last; }
        // 计算每个状态出现次数（endpos 大小）
        occ.assign(sz, 0);
        for (int i = 0; i < (int)s.size(); i++) occ[term[i]]++;
        std::vector<int> order(sz);
        std::iota(order.begin(), order.end(), 0);
        std::sort(order.begin(), order.end(), [&](int a, int b) { return st[a].len > st[b].len; });
        for (int v : order) if (st[v].link != -1) occ[st[v].link] += occ[v];
    }

    // 不同子串个数
    long long distinct_substrings() const {
        long long ans = 0;
        for (int i = 1; i < sz; i++) ans += st[i].len - st[st[i].link].len;
        return ans;
    }

    // 最长子串长度（即原串长度）
    int longest_length() const { return st[last].len; }

    // 模式串 p 在原文中的出现次数（无匹配返回 0）
    long long count(const std::string& p) const {
        int v = 0;
        for (char c : p) {
            auto it = st[v].next.find(c);
            if (it == st[v].next.end()) return 0;
            v = it->second;
        }
        return occ[v];
    }
};

} // namespace better_std
