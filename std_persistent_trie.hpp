#ifndef BETTER_STD_STD_PERSISTENT_TRIE_HPP
#define BETTER_STD_STD_PERSISTENT_TRIE_HPP

//===== 可持久化 01 Trie =====
//  root[i] = 前 i 个元素（a[0..i-1]）构成的 Trie 版本。
//  API：
//    persistent_trie(a, maxbit)   maxbit 为最高位（如 30 表示 2^30）
//    .max_xor(l, r, x)  在 a[l..r]（0-based 闭区间）中找与 x 异或最大值
//    .min_xor(l, r, x)  找异或最小值
//    .kth_xor(l, r, x, k)  a[l..r] 中与 x 异或值第 k 小（k 从 0 起）
#include<vector>
#include<algorithm>
namespace better_std{

class persistent_trie{
    struct node{ int ch[2], sz; };
    std::vector<node> tr;
    std::vector<int> root_;
    int maxbit_;
    int clone_(int p){
        tr.push_back(tr[p]);
        return (int)tr.size()-1;
    }
    // 在 rootR 与 rootL 的差集（即区间 [l,r]）上贪心求异或极值
    int query_extreme_(int R, int L, int x, bool maximize) const {
        int u = root_[R+1], v = root_[L];
        int res = 0;
        for (int i = maxbit_; i >= 0; i--) {
            int b = (x >> i) & 1;
            int want = maximize ? (b^1) : b;   // 想让异或该位为 1（最大）或 0（最小）
            int cntWant = tr[tr[u].ch[want]].sz - tr[tr[v].ch[want]].sz;
            if (cntWant > 0) {
                if (maximize) res |= (1 << i);      // 最大：异或该位为 1
                u = tr[u].ch[want]; v = tr[v].ch[want];
            } else {
                int other = want ^ 1;               // 退而求其次走另一支
                if (!maximize) res |= (1 << i);     // 最小：异或该位被迫为 1
                u = tr[u].ch[other]; v = tr[v].ch[other];
            }
        }
        return res;
    }
    // 第 k 小异或值：按位确定（可达成的前缀决定该位 0/1）
    int kth_xor_(int R, int L, int x, int k) const {
        int u = root_[R+1], v = root_[L];
        int res = 0;
        for (int i = maxbit_; i >= 0; i--) {
            int b = (x >> i) & 1;
            // 该位异或为 0 的个数（选 x 的同位 b）
            int cnt0 = tr[tr[u].ch[b]].sz - tr[tr[v].ch[b]].sz;
            if (k < cnt0) {
                u = tr[u].ch[b]; v = tr[v].ch[b];
            } else {
                k -= cnt0;
                res |= (1 << i);
                u = tr[u].ch[b^1]; v = tr[v].ch[b^1];
            }
        }
        return res;
    }
public:
    persistent_trie() : maxbit_(30) {
        tr.push_back(node{{0,0},0}); root_.push_back(0);
    }
    persistent_trie(const std::vector<int>& a, int maxbit = 30) { build(a, maxbit); }
    void build(const std::vector<int>& a, int maxbit = 30){
        maxbit_ = maxbit;
        tr.clear(); tr.push_back(node{{0,0},0});   // 0 号为空节点
        root_.assign(1, 0);                         // root[0] = 空
        for (int x : a) insert(x);
    }
    void insert(int x){
        int prev = root_.back();
        int cur = clone_(prev);
        tr[cur].sz++;
        int now = cur;
        for (int i = maxbit_; i >= 0; i--) {
            int b = (x >> i) & 1;
            int child = clone_(tr[prev].ch[b]);
            tr[now].ch[b] = child;
            tr[child].sz++;
            prev = tr[prev].ch[b];
            now = child;
        }
        root_.push_back(cur);
    }
    int max_xor(int l, int r, int x) const {
        if (l > r || l < 0 || r >= (int)root_.size()-1) return 0;
        return query_extreme_(r, l, x, true);
    }
    int min_xor(int l, int r, int x) const {
        if (l > r || l < 0 || r >= (int)root_.size()-1) return 0;
        return query_extreme_(r, l, x, false);
    }
    int kth_xor(int l, int r, int x, int k) const {
        if (l > r || l < 0 || r >= (int)root_.size()-1) return 0;
        int len = r - l + 1;
        if (k < 0 || k >= len) return 0;
        return kth_xor_(r, l, x, k);
    }
    int size() const { return (int)root_.size()-1; }
};

} // namespace better_std
#endif // BETTER_STD_STD_PERSISTENT_TRIE_HPP
