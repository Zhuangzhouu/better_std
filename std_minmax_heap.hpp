#pragma once
//===== better_std 双端优先队列（min-max heap）=====
// 一个结构同时充当小根堆与大根堆：push O(log n)，pop_min/pop_max O(log n)
#include<vector>
#include<algorithm>
namespace better_std{

template<typename T>
class std_minmax_heap{
    std::vector<T> h;

    static int level(int i){ return 31 - __builtin_clz(i + 1); }   // 0-based 层级，root=0(最小层)
    static int parent(int i){ return (i - 1) / 2; }
    static int grandparent(int i){ int p = parent(i); return p <= 0 ? -1 : parent(p); }

    void bubble_up_min(int i){
        int gp = grandparent(i);
        if(gp >= 0 && h[i] < h[gp]){ std::swap(h[i], h[gp]); bubble_up_min(gp); }
    }
    void bubble_up_max(int i){
        int gp = grandparent(i);
        if(gp >= 0 && h[i] > h[gp]){ std::swap(h[i], h[gp]); bubble_up_max(gp); }
    }
    void bubble_up(int i){
        if(i == 0) return;
        int p = parent(i);
        if(level(i) % 2 == 0){            // 最小层：若比 max 层父更大则交换，否则与祖父比
            if(h[i] > h[p]){ std::swap(h[i], h[p]); bubble_up_max(p); }
            else bubble_up_min(i);
        } else {                           // 最大层：若比 min 层父更小则交换，否则与祖父比
            if(h[i] < h[p]){ std::swap(h[i], h[p]); bubble_up_min(p); }
            else bubble_up_max(i);
        }
    }

    void trickle_down_min(int i){
        int n = (int)h.size();
        int m = i;
        int cand[6] = {2*i+1, 2*i+2, 4*i+3, 4*i+4, 4*i+5, 4*i+6};
        for(int k = 0; k < 6; k++){ int c = cand[k]; if(c < n && h[c] < h[m]) m = c; }
        if(m == i) return;
        std::swap(h[i], h[m]);
        if(m > 2*i+2){                     // m 是孙子，需与其直接孩子比较
            int sc = m, ch[2] = {2*m+1, 2*m+2};
            if(ch[0] < n && h[ch[0]] < h[sc]) sc = ch[0];
            if(ch[1] < n && h[ch[1]] < h[sc]) sc = ch[1];
            if(sc != m){ std::swap(h[m], h[sc]); trickle_down_min(sc); }
        }
    }
    void trickle_down_max(int i){
        int n = (int)h.size();
        int m = i;
        int cand[6] = {2*i+1, 2*i+2, 4*i+3, 4*i+4, 4*i+5, 4*i+6};
        for(int k = 0; k < 6; k++){ int c = cand[k]; if(c < n && h[c] > h[m]) m = c; }
        if(m == i) return;
        std::swap(h[i], h[m]);
        if(m > 2*i+2){
            int sc = m, ch[2] = {2*m+1, 2*m+2};
            if(ch[0] < n && h[ch[0]] > h[sc]) sc = ch[0];
            if(ch[1] < n && h[ch[1]] > h[sc]) sc = ch[1];
            if(sc != m){ std::swap(h[m], h[sc]); trickle_down_max(sc); }
        }
    }
    void trickle_down(int i){
        if(level(i) % 2 == 0) trickle_down_min(i);
        else trickle_down_max(i);
    }

public:
    void push(const T& x){ h.push_back(x); bubble_up((int)h.size() - 1); }
    bool empty() const { return h.empty(); }
    int size() const { return (int)h.size(); }

    const T& top_min() const { return h[0]; }
    const T& top_max() const {
        if(h.size() == 1) return h[0];
        if(h.size() == 2) return h[1];
        return h[1] > h[2] ? h[1] : h[2];
    }

    void pop_min(){
        if(h.empty()) return;
        if(h.size() == 1){ h.pop_back(); return; }
        h[0] = h.back(); h.pop_back();
        trickle_down_min(0);
    }
    void pop_max(){
        if(h.empty()) return;
        if(h.size() == 1){ h.pop_back(); return; }
        int m = (h.size() == 2) ? 1 : (h[1] > h[2] ? 1 : 2);
        if(m == (int)h.size() - 1){ h.pop_back(); return; }
        h[m] = h.back(); h.pop_back();
        trickle_down(m);
    }
};

} // namespace better_std
