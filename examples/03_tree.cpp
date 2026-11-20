// 示例 3：LCA + Link-Cut Tree + 虚树
#include"../std_better.hpp"
#include<cstdio>
#include<vector>
using namespace better_std;

int main() {
    // 无向树：0-1-2 链，0-3
    std::vector<std::vector<int>> g = {{1, 3}, {0, 2}, {1}, {0}};
    tree_lca lca((int)g.size(), g);
    printf("lca(2,3) = %d (expect 0)\n", lca.lca(2, 3));
    printf("dist(2,3)= %d (expect 3)\n", lca.dist(2, 3));

    // Link-Cut Tree：动态连边 + 路径最大
    link_cut_tree<long long> lct(4);
    for (int i = 0; i < 4; i++) lct.set_val(i, i + 1);
    lct.link(0, 1); lct.link(1, 2); lct.link(2, 3);
    printf("connected(0,3) = %d\n", (int)lct.connected(0, 3));
    printf("path_max(0,3)  = %lld (expect 4)\n", lct.path_max(0, 3));
    lct.path_add(0, 3, 10);
    printf("after add max  = %lld (expect 14)\n", lct.path_max(0, 3));

    // 虚树：关键点 {2,3}，节点数应压缩
    auto vt = virtual_tree(g, lca, std::vector<int>{2, 3});
    printf("virtual tree nodes = %zu\n", vt.second.size());
    return 0;
}
