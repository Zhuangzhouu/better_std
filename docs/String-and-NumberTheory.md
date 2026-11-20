# 字符串与数论

[← 首页](Home.md) · [模块全览](Module-Catalog.md)

## 字符串

### KMP / Z / Manacher

```cpp
std::vector<int> pi = prefix_function(pat);        // KMP 前缀函数
auto occ = kmp_search(text, pat);                  // 所有匹配起点
std::vector<int> z = z_function(s);                // Z 函数
auto occ2 = z_search(text, pat);
std::vector<int> d1, d2;
manacher(s, d1, d2);                               // 奇/偶回文半径
longest_palindrome(s);                             // {起点, 长度}
```

### AC 自动机

```cpp
std_ac_automaton ac;          // 注意：不是模板类
ac.insert("he");
ac.insert("she");
ac.build();                    // 建 fail（BFS）
auto cnt = ac.match("she");   // 各模式出现次数（按插入顺序）
ac.longest_match(text);        // 每个位置的最长匹配模式
```

### 后缀数组 / 自动机

```cpp
suffix_array sa("banana");
sa.sa;   // 后缀起点数组
sa.rk;   // 名次
sa.lcp;  // 高度数组（Kasai）

distinct_substrings(sa);       // 不同子串数
longest_repeated_substring(sa);
kth_distinct_substring(sa, k); // 第 k 小不同子串 {起点, 长度}
```

`suffix_automaton`（SAM）、`palindrome_automaton`（PAM）、`suffix_tree` 见 `std_sa_sam.hpp` / `std_string_extra.hpp`。

### 哈希

```cpp
rolling_hash h("abracadabra");    // 单模
h.get(l, len); h.equal(l1, l2, len);
double_hash dh(s);                // 双模，防碰撞
rollback_hash rb;                 // 可回滚
```

> `get(l,len)` 返回的是规范化子串哈希（高位在前），勿再额外乘 `base^{-(n-l-len)}`。

## 数论

### 基础数论 `std_number_theory.hpp`

```cpp
modpow(a, b, mod);          // 快速幂
exgcd(a, b, x, y);          // 扩展欧几里得，返回 gcd
modinv(a, mod);             // 逆元（mod 素数）
linear_sieve(n);            // 线性筛 → sieve_result
is_prime_ll(n);             // 试除素性
C(n, k, mod); lucas(n, k, p);
bsgs(a, b, mod);            // 离散对数
primitive_root(p);          // 原根
```

### CRT / 线性同余

```cpp
auto r = crt_general({2,3}, {3,5});  // {有解, {最小非负解, 总模数}}
long long x = crt(r_vec, m_vec);      // 互素模数版
linear_congruence(a, b, m);           // 解 ax≡b (mod m)
```

模数可到 1e18，内部用 compat 的防溢出模乘。

### 大整数分解

```cpp
is_prime64(600851475143ULL);          // Miller-Rabin（64 位确定性）
factor_u64(n);                        // Pollard-Rho 分解
prime_factors(n);                     // map<因子, 指数>
divisor_count(n); divisor_sum(n);
```

### 杜教筛 / 线性递推

```cpp
dujiao d;
d.phi_prefix(n);              // Σφ(i), i=1..n
d.mu_prefix(n);               // Σμ(i)

auto rec = berlekamp_massey(seq);     // 最短线性递推
linear_rec_nth(init, rec, n);         // 第 n 项（0-based）
linear_rec_predict(seq, n);           // 直接由序列预测
```

### 组合计数

```cpp
combinatorics<MOD>::C(n, k);
combinatorics<MOD>::catalan(n);
combinatorics<MOD>::stirling1(n, k);
combinatorics<MOD>::stirling2(n, k);
combinatorics<MOD>::derangement(n);

burnside(perms, k);           // Burnside 引理
polya_necklace(n, k);         // 项链染色
polya_bracelet(n, k);         // 手链（考虑翻转）
matrix_tree / spanning_tree_count_mod(n, edges, mod);  // 生成树计数
```

> `berlekamp_massey` 的 discrepancy 已做 `i<=n` 越界保护；`linear_rec_nth` 的 `x^n mod P` 用 2k 截断——都是历史踩坑点，见 [常见问题](FAQ.md)。
