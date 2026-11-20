#ifndef BETTER_STD_STD_SPLAY_HPP
#define BETTER_STD_STD_SPLAY_HPP

//===== Splay 伸展树 =====
//  两种语义：
//   (1) 集合语义：splay_set —— insert/erase/contains/rank/kth/前驱后继，按 key 有序
//   (2) 序列语义：splay_sequence —— 区间翻转/区间和/区间赋值（文艺平衡树）
//  下标 0 为 null 哨兵；所有操作均摊 O(log n)。
#include<vector>
#include<algorithm>
namespace better_std{

//—— 集合语义 Splay（multiset：可含重复键）——
class splay_set{
    struct node{ int ch[2], p, sz, cnt; long long key; };
    std::vector<node> t;
    int root_ = 0;
    void pull(int x){ t[x].sz = t[t[x].ch[0]].sz + t[t[x].ch[1]].sz + t[x].cnt; }
    bool is_child(int x) const { return t[t[x].p].ch[1] == x; }
    void rotate(int x){
        int p = t[x].p, g = t[p].p, d = is_child(x), b = t[x].ch[d^1];
        if (g == 0) root_ = x;                      // 旋转到根：必须更新 root_
        else t[g].ch[t[g].ch[1]==p] = x;
        t[x].p = g;
        t[x].ch[d^1] = p; t[p].p = x;
        t[p].ch[d] = b; if (b) t[b].p = p;
        pull(p); pull(x);
    }
    void splay(int x, int goal=0){
        while (t[x].p != goal) {
            int p = t[x].p, g = t[p].p;
            if (g != goal) rotate((is_child(x)==is_child(p)) ? p : x);
            rotate(x);
        }
        pull(x);
    }
    int new_node(long long key){
        t.push_back(node());
        int id = (int)t.size()-1;
        t[id].ch[0]=t[id].ch[1]=t[id].p=0;
        t[id].sz=t[id].cnt=1; t[id].key=key;
        return id;
    }
    // 返回 >=key 的第一个节点（或 0）
    int lower_bound_(long long key){
        int u = root_, res = 0;
        while (u) {
            if (t[u].key >= key) { res = u; u = t[u].ch[0]; }
            else u = t[u].ch[1];
        }
        return res;
    }
public:
    splay_set(){ t.push_back(node()); t[0].ch[0]=t[0].ch[1]=t[0].p=t[0].sz=t[0].cnt=0; t[0].key=0; }
    void insert(long long key){
        if (!root_) { root_ = new_node(key); return; }
        int u = root_, p = 0;
        while (u) {
            p = u;
            if (key == t[u].key) { splay(u); t[u].cnt++; pull(u); return; }
            u = t[u].ch[key < t[u].key ? 0 : 1];
        }
        int id = new_node(key);
        t[p].ch[key < t[p].key ? 0 : 1] = id;
        t[id].p = p;
        splay(id);
    }
    bool erase(long long key){
        int u = lower_bound_(key);
        if (!u || t[u].key != key) return false;
        splay(u);
        if (t[u].cnt > 1) { t[u].cnt--; pull(u); return true; }
        int L = t[u].ch[0], R = t[u].ch[1];
        if (!R) {                          // 无右子树：左子树顶替
            root_ = L; if (L) t[L].p = 0;
        } else if (!L) {
            root_ = R; t[R].p = 0;
        } else {
            // splay 前驱（L 中最大）到 L 根，再把 R 挂到其右
            int mx = L; while (t[mx].ch[1]) mx = t[mx].ch[1];
            // 先把 u 摘掉：根变为 L，u 的子树断开
            root_ = L; t[L].p = 0;
            splay(mx);                      // mx 成为整树根，其右子树为空
            t[mx].ch[1] = R; t[R].p = mx;
            pull(mx);
        }
        return true;
    }
    bool contains(long long key){ int u = lower_bound_(key); if(u) splay(u); return u && t[u].key==key; }
    // 严格小于 key 的元素个数（含重复）
    int rank(long long key){
        int u = lower_bound_(key), res = 0, cur = root_;
        if (u) splay(u);
        // splay 后 u 为根，其左子树即所有 < key 的元素
        if (u) res = t[t[u].ch[0]].sz;
        else res = t[root_].sz;  // 所有元素都 < key
        (void)cur;
        return res;
    }
    // 第 k 小（1-based），返回 key；不存在返回 0
    long long kth(int k){
        int u = root_;
        if (k < 1 || k > t[u].sz) return 0;
        while (u) {
            int ls = t[t[u].ch[0]].sz;
            if (k <= ls) u = t[u].ch[0];
            else if (k <= ls + t[u].cnt) return t[u].key;
            else { k -= ls + t[u].cnt; u = t[u].ch[1]; }
        }
        return 0;
    }
    long long predecessor(long long key){
        int u = lower_bound_(key);
        if (u) splay(u);
        u = t[root_].ch[0];
        if (!u) return 0;
        while (t[u].ch[1]) u = t[u].ch[1];
        splay(u);
        return t[u].key;
    }
    long long successor(long long key){
        int u = lower_bound_(key);
        if (!u) return 0;
        splay(u);
        if (t[u].cnt > 1) return t[u].key;
        u = t[u].ch[1];
        if (!u) return 0;
        while (t[u].ch[0]) u = t[u].ch[0];
        splay(u);
        return t[u].key;
    }
    int size() const { return t[root_].sz; }
    bool empty() const { return root_ == 0; }
};

//—— 序列语义 Splay（区间翻转 / 区间和）——
class splay_sequence{
    struct node{ int ch[2], p, sz; long long val, sum; bool rev; };
    std::vector<node> t;
    int root_ = 0;
    void pull(int x){
        t[x].sz = t[t[x].ch[0]].sz + t[t[x].ch[1]].sz + 1;
        t[x].sum = t[t[x].ch[0]].sum + t[t[x].ch[1]].sum + t[x].val;
    }
    void apply_rev(int x){ if(!x) return; t[x].rev = !t[x].rev; std::swap(t[x].ch[0], t[x].ch[1]); }
    void push(int x){ if (t[x].rev){ apply_rev(t[x].ch[0]); apply_rev(t[x].ch[1]); t[x].rev=false; } }
    bool is_child(int x) const { return t[t[x].p].ch[1] == x; }
    void rotate(int x){
        int p = t[x].p, g = t[p].p, d = is_child(x), b = t[x].ch[d^1];
        if (g == 0) root_ = x;                      // 旋转到根：必须更新 root_
        else t[g].ch[t[g].ch[1]==p] = x;
        t[x].p = g;
        t[x].ch[d^1] = p; t[p].p = x;
        t[p].ch[d] = b; if (b) t[b].p = p;
        pull(p); pull(x);
    }
    void splay(int x, int goal=0){
        static std::vector<int> stk; stk.clear();
        int y = x; stk.push_back(y); while (t[y].p != goal){ y = t[y].p; stk.push_back(y); }
        for (int i=(int)stk.size()-1;i>=0;i--) push(stk[i]);
        while (t[x].p != goal) {
            int p = t[x].p, g = t[p].p;
            if (g != goal) rotate((is_child(x)==is_child(p)) ? p : x);
            rotate(x);
        }
        pull(x);
    }
    int new_node(long long v){
        t.push_back(node());
        int id=(int)t.size()-1;
        t[id].ch[0]=t[id].ch[1]=t[id].p=0;
        t[id].sz=1; t[id].val=t[id].sum=v; t[id].rev=false;
        return id;
    }
    // 找到中序第 k 个节点（1-based）
    int kth_node(int k){
        int u = root_;
        while (u) {
            push(u);
            int ls = t[t[u].ch[0]].sz;
            if (k <= ls) u = t[u].ch[0];
            else if (k == ls+1) return u;
            else { k -= ls+1; u = t[u].ch[1]; }
        }
        return 0;
    }
public:
    splay_sequence(){ t.push_back(node()); t[0].ch[0]=t[0].ch[1]=t[0].p=t[0].sz=0; t[0].val=t[0].sum=0; t[0].rev=false; }
    explicit splay_sequence(const std::vector<long long>& a){ build(a); }
    // O(n) 递归平衡构造（按中点建树，保证 O(log n) 高度）
    void build(const std::vector<long long>& a){
        t.clear(); t.push_back(node());
        t[0].ch[0]=t[0].ch[1]=t[0].p=t[0].sz=0; t[0].val=t[0].sum=0; t[0].rev=false;
        root_ = build_balanced(a, 0, (int)a.size()-1, 0);
    }
private:
    int build_balanced(const std::vector<long long>& a, int l, int r, int p){
        if (l > r) return 0;
        int mid = (l+r)>>1;
        int id = new_node(a[mid]);
        t[id].p = p;
        int left = build_balanced(a, l, mid-1, id);
        int right = build_balanced(a, mid+1, r, id);
        t[id].ch[0] = left; t[id].ch[1] = right;
        pull(id);
        return id;
    }
public:
    int size() const { return t[root_].sz; }
    long long range_sum(int l, int r){    // 1-based 闭区间
        if (l > r || l < 1 || r > size()) return 0;
        return range_sum_prefix(r) - range_sum_prefix(l-1);
    }
    long long range_sum_prefix(int k){
        if (k <= 0) return 0;
        if (k >= size()) return t[root_].sum;
        int x = kth_node(k); splay(x);
        return t[t[x].ch[0]].sum + t[x].val;
    }
    void reverse_range(int l, int r){    // 1-based 闭区间
        if (l >= r || l < 1 || r > size()) return;
        // 用 split：先把 r+1 splay 到根，再把 l-1 splay 到 r+1 的左子树
        int rr = (r+1 <= size()) ? kth_node(r+1) : 0;
        int ll = (l-1 >= 1) ? kth_node(l-1) : 0;
        if (rr) { splay(rr); }
        if (ll) { splay(ll, rr ? rr : 0); }
        int target = rr ? t[rr].ch[0] : (ll ? t[ll].ch[1] : root_);
        apply_rev(target);
    }
    std::vector<long long> to_vector(){
        std::vector<long long> res; res.reserve(size());
        // 中序递归（含 push）
        std::vector<std::pair<int,int>> st; st.push_back({root_,0});
        while (!st.empty()) {
            int u=st.back().first, idx=st.back().second;
            if (u==0){ st.pop_back(); continue; }
            if (idx==0) push(u);
            if (idx==0){ st.back().second=1; st.push_back({t[u].ch[0],0}); }
            else if (idx==1){ st.back().second=2; res.push_back(t[u].val); st.push_back({t[u].ch[1],0}); }
            else st.pop_back();
        }
        return res;
    }
};

} // namespace better_std
#endif // BETTER_STD_STD_SPLAY_HPP
