// 示例 4：CRT + 大整数分解 + 组合数 + BM 线性递推
#include"../std_better.hpp"
#include<cstdio>
#include<vector>
using namespace better_std;

int main() {
    // CRT：x ≡ 2 (mod 3), x ≡ 3 (mod 5) → x ≡ 8 (mod 15)
    auto r = crt_general({2, 3}, {3, 5});
    printf("crt      = %lld (mod %lld), expect 8 (mod 15)\n",
           r.second.first, r.second.second);

    // 大整数分解（Miller-Rabin + Pollard-Rho）
    unsigned long long n = 600851475143ULL;
    auto fac = prime_factors(n);
    printf("factors  =");
    for (auto& kv : fac) printf(" %llu^%d", kv.first, kv.second);
    printf("  count=%llu\n", divisor_count(n));

    // BM 线性递推：斐波那契 → 第 30 项
    std::vector<int> fib = {0, 1};
    for (int i = 2; i < 30; i++) fib.push_back(fib[i-1] + fib[i-2]);
    auto rec = berlekamp_massey(fib);
    printf("fib[30]  = %d (BM rec len=%zu, expect 832040)\n",
           linear_rec_nth(fib, rec, 30), rec.size());
    return 0;
}
