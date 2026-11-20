// 示例 2：最大流 + 2-SAT
#include"../std_better.hpp"
#include<cstdio>
#include<vector>
using namespace better_std;

int main() {
    // Dinic 最大流：经典 4 点网络，最大流 = 2
    std_dinic<long long> dinic(4);
    dinic.add_edge(0, 1, 3);
    dinic.add_edge(0, 2, 2);
    dinic.add_edge(1, 3, 2);
    dinic.add_edge(2, 3, 3);
    printf("maxflow = %lld (expect 4)\n", dinic.max_flow(0, 3));

    // 2-SAT：子句 (x0 ∨ x1) ∧ (¬x0 ∨ x1) ∧ (x0 ∨ ¬x1)
    two_sat sat;
    sat.reset(2);
    sat.add(0, true,  1, true);
    sat.add(0, false, 1, true);
    sat.add(0, true,  1, false);
    std::vector<char> assign;
    bool ok = sat.satisfiable(&assign);
    printf("2sat    = %s", ok ? "SAT" : "UNSAT");
    if (ok) printf("  assign=(%d,%d)", (int)assign[0], (int)assign[1]);
    printf("\n");
    return 0;
}
