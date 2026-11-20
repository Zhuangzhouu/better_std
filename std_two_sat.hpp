#ifndef BETTER_STD_STD_TWO_SAT_HPP
#define BETTER_STD_STD_TWO_SAT_HPP

//===== better_std 2-SAT（Tarjan SCC）=====
//   变量 0..n-1，每个变量两个点：2*i(false)、2*i+1(true)。
//   子句 (a==va) || (b==vb) 用 add(a,va,b,vb) 添加两条蕴含边。
//   可满足性：x 与 ¬x 不在同一 SCC；赋值由 SCC 拓扑序得出。
#include<vector>
#include<algorithm>
namespace better_std{

struct two_sat{
    int n;
    std::vector<std::vector<int>> g, rg;
    explicit two_sat(int n_=0):n(n_),g(2*n_),rg(2*n_){}
    void reset(int n_){ n=n_; g.assign(2*n_,{}); rg.assign(2*n_,{}); }
    int node(int var,bool val) const { return 2*var + (val?1:0); }
    void add_imp(int u,int v){ g[u].push_back(v); rg[v].push_back(u); }
    // 加子句 (varA==valA) || (varB==valB)
    void add(int a,bool va,int b,bool vb){
        int u=node(a,va), v=node(b,vb);
        add_imp(u^1, v);   // ¬a ⇒ b
        add_imp(v^1, u);   // ¬b ⇒ a
    }
    // 仅约束 var==val（单元子句）
    void add_unit(int a,bool val){ add_imp(node(a,!val), node(a,val)); }

    // 返回是否可满足；若满足，assignment[i] 给出一组赋值
    bool satisfiable(std::vector<char>* assignment=nullptr) const {
        int m=2*n;
        std::vector<int> vis(m,0), order, comp(m,-1);
        // 第一遍 DFS 求完成序
        for(int s=0;s<m;s++) if(!vis[s]){
            std::vector<std::pair<int,size_t>> st;
            st.push_back({s,0}); vis[s]=1;
            while(!st.empty()){
                int u=st.back().first; size_t& i=st.back().second;
                if(i<g[u].size()){ int v=g[u][i++]; if(!vis[v]){ vis[v]=1; st.push_back({v,0}); } }
                else { order.push_back(u); st.pop_back(); }
            }
        }
        // 第二遍在反图上按逆序染色
        int cc=0;
        for(int k=m-1;k>=0;k--){
            int s=order[k]; if(comp[s]!=-1) continue;
            std::vector<int> st={s}; comp[s]=cc;
            while(!st.empty()){
                int u=st.back(); st.pop_back();
                for(int v:rg[u]) if(comp[v]==-1){ comp[v]=cc; st.push_back(v); }
            }
            cc++;
        }
        for(int i=0;i<n;i++) if(comp[2*i]==comp[2*i+1]) return false;
        if(assignment){
            assignment->assign(n,0);
            // comp 按反图逆序编号：编号越大拓扑越靠前；取真值所在 SCC 编号较大者
            for(int i=0;i<n;i++) (*assignment)[i] = (char)(comp[2*i+1] > comp[2*i]);
        }
        return true;
    }
};

} // namespace better_std
#endif // BETTER_STD_STD_TWO_SAT_HPP
