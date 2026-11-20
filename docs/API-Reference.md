# API 速查

[← 首页](Home.md)

按类别汇总高频函数/类签名。下标约定见 [常见问题](FAQ.md)。

## 数值 / 数学

```cpp
modpow(a, b, mod)                 // ll 快速幂
exgcd(a, b, x, y) -> gcd          // 扩展欧几里得
modinv(a, mod)                    // 逆元
mul_mod_64(a, b, m)               // 防溢出模乘（compat）
ilog2_32(x) / popcount64(x)       // compat 位运算回退
High_precision_number<A>(str)     // 高精度整数
High_precision_floating_point_number<A>
fraction / std_complex / matrix<T>
```

## 排序 / 二分

```cpp
quick_sort / merge_sort / heap_sort / insertion_sort / selection_sort / shell_sort(first,last[,cmp])
counting_sort(v) / radix_sort(v) / bucket_sort(v,lo,hi)
nth_element(first,last,k[,cmp]) / quickselect(...)
first_true(f,lo,hi) / last_true(f,lo,hi)          // 二分谓词
ternary_search_min / ternary_search_max(f,lo,hi)  // 实数三分
ternary_search_min_discrete / _max_discrete(...)  // 整数三分
```

## 数据结构

```cpp
std_fenwick<T>(n): add(i,v) sum(i) range_sum(l,r) kth(k)
std_segment_tree<T>(n): build update query range_add
std_segment_tree_beats: range_chmin range_add
std_sparse_table: build query(l,r)
std_persistent_segment_tree: 主席树第 k 小
cat_tree<T,Op>(a,op): query(l,r,op)
std_bst<T>: insert erase contains kth rank count_range predecessor successor inorder min max
std_heap<T> / std_minmax_heap<T>
std_trie
std_dsu: find unite same
rollback_dsu / persistent_dsu / weighted_dsu<Monoid> / type_dsu / interval_dsu
leftist_heap<T> / skew_heap<T> / pairing_heap<T>: push top pop merge empty size
fhq_treap: insert erase rank_of kth reverse_range build build_set
sqrt_decomposition: range_add range_sum range_max count_leq
std_linear_basis / prefix_linear_basis
```

## 字符串

```cpp
prefix_function(s) / kmp_search(text,pat)
z_function(s) / z_search(text,pat)
manacher(s,d1,d2) / longest_palindrome(s) -> {pos,len}
std_ac_automaton: insert build match longest_match covered
suffix_array(s): sa rk lcp
distinct_substrings(sa) / longest_repeated_substring(sa) / kth_distinct_substring(sa,k)
suffix_automaton / palindrome_automaton / suffix_tree
rolling_hash(s) / double_hash(s) / rollback_hash: get equal
std_string_hash
```

## 图 / 流 / 树

```cpp
std_dinic<T>(n): add_edge(u,v,cap) max_flow(s,t)
std_mcmf: 最小费用最大流
hopcroft_karp: add(u,v) max_matching()
dinic (bipartite_matching.hpp 内): maxflow
km_max_weight(w,nL,nR) / km_min_weight(...) -> km_result{match,weight}
tarjan(g) -> tarjan_result{is_cut,bridges,...}
two_sat: reset add add_unit satisfiable(&assign)
std_grid / Grid_adjacency_list / Grid_adjacency_matrix: add_edge set_edge
std_rootless_tree / std_tree / std_binary_tree / std_heavy_light
tree_lca(n,g): lca dist dfn
virtual_tree(g,lca,key) -> {邻接表,节点}
tree_diameter(g,u,v) -> 直径边数（u,v 带出端点）/ tree_centroid(g)
block_cut_tree(g): size dist / cactus_dist(g,u,v)
link_cut_tree<T>(n): link cut connected makeroot path_add path_max set_val
```

## 数论 / 组合

```cpp
linear_sieve(n) -> sieve_result
is_prime_ll(n) / is_prime64(n)
C(n,k,mod) / lucas(n,k,p) / bsgs(a,b,m) / primitive_root(p)
crt(r,m) / crt_general(r,m) / linear_congruence(a,b,m) / merge(a1,m1,a2,m2)
factor_u64(n) / prime_factors(n) / divisor_count(n) / divisor_sum(n)
dujiao: phi_prefix(n) mu_prefix(n)
berlekamp_massey(s) / linear_rec_nth(init,rec,n) / linear_rec_predict(s,n)
combinatorics<MOD>: C catalan derangement stirling1 stirling2
burnside(perms,k) / polya_necklace(n,k) / polya_bracelet(n,k)
gauss_solve(A,b) / gauss_mod(A,b,mod) / xor_gauss(eq,vars,sol)
spanning_tree_count_mod(n,edges,mod)
knapsack_01 / knapsack_complete / knapsack_multiple / subset_sum
```

## 多项式 / 卷积

```cpp
ntt(a,invert,mod,root) / fft(a,invert) / fft_multiply(a,b)
poly_inv(a,n,mod) / poly_ln / poly_exp
multipoint_eval(f,xs) / interpolate(xs,ys)
factorize(expr)
fwt_and / fwt_or / fwt_xor(a,invert)
and_conv / or_conv / xor_conv(a,b)
fst_subset / fst_superset(a,invert) / subset_conv(f,g,n)
```

## 几何 / 数值 / 其他

```cpp
point2<T> / convex_hull(pts) / polygon_area(pts)
on_segment(a,b,p) / rectangle_union_area(rects)
simpson / adaptive_simpson / bisect_root / newton_root
lagrange_interpolate(xs,ys,x0) / lagrange_interpolate_mod(ys,x0)
simplex(A,b,c) -> simplex_result
kd_tree<T>(pts): nearest(q) range_count(lo,hi)
overall_binary_range_kth(a,qs,vlo,vhi)
segment_tree_divide_bipartite(n,T,edges)
cdq_3d_partial_order(pts) / cdq_divide_conquer(a,l,r,merge)
lis(a) / lcs(a,b) / lis_sequence / lcs_sequence
largest_rectangle(h) / trapping_rain_water(h) / mono_queue_max/min(a,k)
next_perm / prev_perm / perm_to_rank / rank_to_perm / inversion_count
discretize / compress / rank_of
two_sum / three_sum / three_sum_closest / three_sum_count / four_sum / three_sum_exists_bitset
nim_xor / nim_win / nim_first_move / sg_dag / bash_game / stairs_nim
```

> 本页是速查，参数细节以各头文件注释为准；完整分类见 [模块全览](Module-Catalog.md)。
