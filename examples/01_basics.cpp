// 示例 1：高精度 + 排序 + 后缀数组 + 多项式因式分解
#include"../std_better.hpp"
#include<cstdio>
#include<string>
#include<vector>
using namespace better_std;

int main() {
    // 高精度整数：精度作为模板参数，支持字符串构造与流输出
    High_precision_number<50> a("12345678901234567890"), b(7);
    std::cout << "hpn  = " << (a * b + b) << "\n";

    // 排序
    std::vector<int> v = {5, 3, 9, 1, 7, 2};
    quick_sort(v.begin(), v.end());
    printf("sort =");
    for (int x : v) printf(" %d", x);
    printf("\n");

    // 后缀数组：sa[i] 是第 i 小后缀的起点
    suffix_array sa("banana");
    printf("sa   =");
    for (int x : sa.sa) printf(" %d", x);
    printf("\n");

    // 多项式因式分解
    std::string f = factorize("x^2-1");
    printf("poly = %s\n", f.c_str());
    return 0;
}
