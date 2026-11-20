// 示例 5：AC 自动机 + 双哈希 + 3SUM
#include"../std_better.hpp"
#include<cstdio>
#include<string>
#include<vector>
using namespace better_std;

int main() {
    // AC 自动机：多模式匹配计数
    std_ac_automaton ac;
    ac.insert("he"); ac.insert("she"); ac.insert("e");
    ac.build();
    auto cnt = ac.match("she");
    printf("ac match = she:%d he:%d e:%d\n", cnt[0], cnt[1], cnt[2]);

    // 双哈希：判断子串相等
    double_hash h("abracadabra");
    printf("hash eq  = %d (\"ab\"@0 vs \"ab\"@7)\n",
           (int)h.equal(0, 7, 2));

    // 3SUM 计数：不同下标三元组和 = target
    std::vector<int> a = {1, 1, 2, 3};
    printf("3sum cnt = %lld (target=4, expect 1)\n",
           three_sum_count(a, 4));
    return 0;
}
