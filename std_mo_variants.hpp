#ifndef BETTER_STD_STD_MO_VARIANTS_HPP
#define BETTER_STD_STD_MO_VARIANTS_HPP

//===== 莫队变体 =====
//  用户提供一个"状态"结构体 S，需实现：
//    void add(int pos)    在右端加入位置 pos
//    void remove(int pos) 从左端移除位置 pos
//    S 类型须可默认构造/可复制。
//  1) modified_mo<S>   —— 带修莫队：支持单点修改，查询 [l,r] 在时刻 t 的状态
//     修改操作：{pos, newval}；查询操作：{l, r, t}
//     状态还需 void apply_modify(int pos, int newval) / undo_modify(int pos,int oldval)，
//     二者**只更新底层数组、不更新统计值**（库会在区间内时先 remove 再 add 来维护统计）。
//  2) rollback_mo<S>   —— 回滚莫队：只增不减的 add，左端用回滚，适合难以 remove 的统计
//     查询 [l,r]；状态只需 add(pos) 与一个可复制快照（S 可拷贝）。
#include<vector>
#include<algorithm>
#include<cmath>
#include<utility>
namespace better_std{

namespace mo_detail{
    struct query{ int l, r, t, id; };
    struct modify{ int pos, nv, ov; };
    inline int block_size(int n){ return std::max(1, (int)std::sqrt(n)); }
}

//—— 带修莫队 ——
//  n 元素；mods 为修改序列（pos 0-based, nv/ov 新/旧值）。
//  queries: {l, r, t, id} —— 0-based 闭区间 [l,r]，t 为应用前 t 个修改（1..mods.size()）。
//  返回每个查询结束时的状态副本 vector<S>。
template<typename S>
std::vector<S> modified_mo(int n,
        const std::vector<mo_detail::modify>& mods,
        const std::vector<mo_detail::query>& qs,
        S init){
    using namespace mo_detail;
    int B = block_size(std::max(1,n));
    int M = (int)mods.size();
    std::vector<query> ord(qs);
    std::sort(ord.begin(), ord.end(), [&](const query& a, const query& b){
        int ba = a.l / B, bb = b.l / B;
        if (ba != bb) return ba < bb;
        int ra = a.r / B, rb = b.r / B;
        if (ra != rb) return ra < rb;
        return a.t < b.t;
    });
    std::vector<S> ans(qs.size(), init);
    S cur = init;
    int L = 0, R = -1, T = 0;
    for (const query& q : ord) {
        while (T < q.t) {
            const modify& m = mods[T];
            if (L <= m.pos && m.pos <= R) cur.remove(m.pos);   // 先用旧值移出窗口
            cur.apply_modify(m.pos, m.nv);                     // 只改底层数组，不碰统计
            if (L <= m.pos && m.pos <= R) cur.add(m.pos);      // 再用新值加入窗口
            T++;
        }
        while (T > q.t) {
            T--;
            const modify& m = mods[T];
            if (L <= m.pos && m.pos <= R) cur.remove(m.pos);
            cur.undo_modify(m.pos, m.ov);
            if (L <= m.pos && m.pos <= R) cur.add(m.pos);
        }
        while (R < q.r) { ++R; cur.add(R); }
        while (R > q.r) { cur.remove(R); --R; }
        while (L > q.l) { --L; cur.add(L); }
        while (L < q.l) { cur.remove(L); ++L; }
        ans[q.id] = cur;
    }
    return ans;
}

//—— 回滚莫队 ——
//  查询 [l,r]；状态 S 只需 add(pos)，可拷贝作为快照。
//  对左端在同一块内的查询：右端单向扩展，左端用快照回滚；跨块查询用临时状态。
//  返回 vector<S>。
template<typename S>
std::vector<S> rollback_mo(int n, const std::vector<std::pair<int,int>>& queries, S init){
    using namespace mo_detail;
    int B = block_size(std::max(1,n));
    int Q = (int)queries.size();
    std::vector<int> ord(Q);
    for (int i = 0; i < Q; i++) ord[i] = i;
    // 按左端块排序，同块按右端升序（右端单向扩展，左端靠快照回滚）
    std::sort(ord.begin(), ord.end(), [&](int i, int j){
        int bi = queries[i].first / B, bj = queries[j].first / B;
        if (bi != bj) return bi < bj;
        return queries[i].second < queries[j].second;
    });
    std::vector<S> ans(Q, init);
    size_t idx = 0;
    while (idx < ord.size()) {
        int block = queries[ord[idx]].first / B;
        int blockEnd = std::min(n-1, (block+1)*B - 1);   // 本块右边界
        S base = init;                                    // 右端已扩展的基准状态
        int R = blockEnd;                                 // 右端指针（只增）
        size_t j = idx;
        for (; j < ord.size() && queries[ord[j]].first / B == block; j++) {
            int l = queries[ord[j]].first, r = queries[ord[j]].second;
            if (r <= blockEnd) {
                // 短查询：左右端都在块内，直接临时暴力（O(B)）
                S tmp = init;
                for (int p = l; p <= r; p++) tmp.add(p);
                ans[ord[j]] = tmp;
                continue;
            }
            while (R < r) { ++R; base.add(R); }           // 右端单向扩展（base 只含右端，永不被污染）
            S query_state = base;                          // 复制快照：纯右端状态
            for (int p = blockEnd; p >= l; p--) query_state.add(p);  // 左端从块尾向左补
            ans[ord[j]] = query_state;                     // base 不变 = 自动回滚
        }
        idx = j;
    }
    return ans;
}

} // namespace better_std
#endif // BETTER_STD_STD_MO_VARIANTS_HPP
