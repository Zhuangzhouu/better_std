#pragma once
//===== better_std::std_string_extra —— 回文自动机 / 后缀树 / 后缀数组 height 应用 =====
//  palindrome_automaton   : 回文自动机（Eertree），distinct/longest/count_occurrences/总数
//  suffix_tree            : 由后缀数组 + LCP 栈式构建的紧凑后缀树（O(n)），
//                           distinct_substrings / longest_repeat_len / 节点遍历
//  后缀数组 height 应用     : distinct_substrings / longest_repeated_substring /
//                           kth_distinct_substring / longest_common_substring
#include <vector>
#include <string>
#include <array>
#include <algorithm>
#include <numeric>
#include <utility>
#include "std_sa_sam.hpp"
namespace better_std {

//==================== 回文自动机（Eertree）=====================
struct palindrome_automaton {
    struct node {
        std::array<int, 26> next;
        int len, link;
        long long cnt;
    };
    std::vector<node> st;
    std::string s;
    int last;

    palindrome_automaton() { reset(); }

    void reset() {
        st.clear();
        st.push_back({{}, -1, 0, 0});   // 奇根 len=-1
        st.push_back({{}, 0, 0, 0});    // 偶根 len=0, link=0
        last = 1;
        s.clear();
    }

    void extend(char ch) {
        int c = ch - 'a';
        int pos = (int)s.size();
        s.push_back(ch);
        int cur = last;
        while (true) {
            int l = st[cur].len;
            int left = pos - 1 - l;
            if (left >= 0 && s[left] == ch) break;
            cur = st[cur].link;
        }
        if (st[cur].next[c]) {
            last = st[cur].next[c];
            st[last].cnt++;
            return;
        }
        int nn = (int)st.size();
        st.push_back({{}, st[cur].len + 2, 0, 1});
        st[cur].next[c] = nn;
        if (st[nn].len == 1) {
            st[nn].link = 1;
        } else {
            int q = st[cur].link;
            while (true) {
                int l = st[q].len;
                int left = pos - 1 - l;
                if (left >= 0 && s[left] == ch) break;
                q = st[q].link;
            }
            st[nn].link = st[q].next[c];
        }
        last = nn;
    }

    void build(const std::string& t) {
        reset();
        for (char ch : t) extend(ch);
    }

    int distinct_palindromes() const { return (int)st.size() - 2; }
    int longest_palindrome_len() const {
        int mx = 0;
        for (size_t i = 2; i < st.size(); i++) mx = std::max(mx, st[i].len);
        return mx;
    }
    // 统计每个回文的出现次数（build 后调用一次，cnt 变为 endpos 大小）
    void count_occurrences() {
        std::vector<int> order(st.size());
        std::iota(order.begin(), order.end(), 0);
        std::sort(order.begin() + 2, order.end(),
                  [&](int a, int b) { return st[a].len > st[b].len; });
        for (int i : order)
            if (i >= 2) st[st[i].link].cnt += st[i].cnt;
    }
    long long total_palindromic_substrings() const {
        long long sum = 0;
        for (size_t i = 2; i < st.size(); i++) sum += st[i].cnt;
        return sum;
    }
};

//==================== 后缀树（SA + LCP 栈式构建）=====================
// 节点：0 为根；parent[i] 父节点（根为 -1）；depth[i] 串深度；
//       leaf[i] = 叶子对应后缀起点（非叶子 -1）；children 邻接表。
struct suffix_tree {
    std::string s;
    int n;
    std::vector<int> parent, depth, leaf;
    std::vector<std::vector<int>> children;
    int root;

    explicit suffix_tree(const std::string& str) : s(str), n((int)str.size()) {
        root = 0;
        parent.push_back(-1); depth.push_back(0); leaf.push_back(-1);
        if (n == 0) return;
        suffix_array SA(s);
        // 栈式构建（参照 Kasai et al. 从 SA+LCP 线性建后缀树）
        std::vector<int> stk;
        stk.push_back(root);
        auto new_node = [&](int par, int d) {
            parent.push_back(par); depth.push_back(d); leaf.push_back(-1);
            return (int)parent.size() - 1;
        };
        for (int i = 0; i < n; i++) {
            int l = (i == 0) ? 0 : SA.lcp[i];
            int last_popped = -1;
            while (depth[stk.back()] > l) { last_popped = stk.back(); stk.pop_back(); }
            if (depth[stk.back()] < l) {
                int v = new_node(stk.back(), l);
                if (last_popped != -1) parent[last_popped] = v;
                stk.push_back(v);
            }
            int leaf_node = new_node(stk.back(), n - SA.sa[i]);
            leaf[leaf_node] = SA.sa[i];
            stk.push_back(leaf_node);
        }
        children.assign(parent.size(), {});
        for (size_t i = 1; i < parent.size(); i++) children[parent[i]].push_back((int)i);
    }

    // 不同子串数 = 所有边长度之和（depth 差）
    long long distinct_substrings() const {
        long long sum = 0;
        for (size_t i = 1; i < parent.size(); i++) sum += depth[i] - depth[parent[i]];
        return sum;
    }
    // 最长重复子串长度 = 子树叶子数 ≥2 的节点最大串深度
    // （无哨兵构建下"叶子兼内部节点"合法：如 "ab" 既是叶子又是 "abab" 的父节点）
    int longest_repeat_len() const {
        int nc = (int)parent.size();
        if (nc <= 1) return 0;
        std::vector<int> lc(nc, 0);
        std::vector<int> order;
        order.reserve(nc);
        std::vector<int> stk = {0};
        while (!stk.empty()) {
            int u = stk.back(); stk.pop_back();
            order.push_back(u);
            for (int v : children[u]) stk.push_back(v);
        }
        for (int i = (int)order.size() - 1; i >= 0; i--) {
            int u = order[i];
            if (leaf[u] != -1) lc[u] = 1;
            for (int v : children[u]) lc[u] += lc[v];
        }
        int mx = 0;
        for (int i = 0; i < nc; i++)
            if (lc[i] >= 2) mx = std::max(mx, depth[i]);
        return mx;
    }
    int node_count() const { return (int)parent.size(); }
};

//==================== 后缀数组 height 应用 ====================
// 不同子串数：n(n+1)/2 - sum(lcp)
inline long long distinct_substrings(const suffix_array& SA) {
    long long total = (long long)SA.n * (SA.n + 1) / 2;
    for (int i = 1; i < SA.n; i++) total -= SA.lcp[i];
    return total;
}
// 最长重复子串（出现至少 2 次）：max(lcp)
inline int longest_repeated_substring(const suffix_array& SA) {
    int mx = 0;
    for (int i = 1; i < SA.n; i++) mx = std::max(mx, SA.lcp[i]);
    return mx;
}
// 字典序第 k 小（0-based）的不同子串，返回 {起始下标, 长度}；k 越界返回 {-1,-1}
inline std::pair<int, int> kth_distinct_substring(const suffix_array& SA, long long k) {
    for (int i = 0; i < SA.n; i++) {
        long long cnt = (long long)(SA.n - SA.sa[i]) - SA.lcp[i];
        if (k < cnt) return {SA.sa[i], (int)(k + SA.lcp[i] + 1)};   // 新增子串长度从 lcp+1 起
        k -= cnt;
    }
    return {-1, -1};
}
// 两串最长公共子串：返回 {长度, 在拼接串 a + sep + b 中的起始位置}；sep 须为 a/b 中不出现的字符
inline std::pair<int, int> longest_common_substring(const std::string& a, const std::string& b,
                                                    char sep = '#') {
    if (a.empty() || b.empty()) return {0, -1};
    std::string t = a + sep + b;
    suffix_array SA(t);
    int na = (int)a.size();
    int best = 0, pos = -1;
    for (int i = 1; i < SA.n; i++) {
        int x = SA.sa[i - 1], y = SA.sa[i];
        bool diff = (x < na) != (y < na);
        if (diff && SA.lcp[i] > best) {
            best = SA.lcp[i];
            pos = (x < na) ? x : y;
        }
    }
    return {best, pos};
}

} // namespace better_std
