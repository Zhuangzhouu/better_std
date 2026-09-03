#pragma once
//===== better_std 数论工具包（long long 模运算）=====
#include<cstdint>
#include<vector>
#include<algorithm>
#include<unordered_map>
#include<cmath>
namespace better_std{

using ll = long long;

// 快速幂 a^b mod m（__int128 防溢出，m 任意正整数）
ll modpow(ll a, ll b, ll m){
    if(m == 1) return 0;
    a %= m; if(a < 0) a += m;
    ll r = 1 % m;
    while(b){
        if(b & 1) r = (__int128)r * a % m;
        a = (__int128)a * a % m;
        b >>= 1;
    }
    return r;
}

// 扩展欧几里得：ax+by=gcd(a,b)，返回 gcd；x,y 为引用输出
ll exgcd(ll a, ll b, ll& x, ll& y){
    if(b == 0){ x = 1; y = 0; return a; }
    ll x1, y1;
    ll g = exgcd(b, a % b, x1, y1);
    x = y1;
    y = x1 - (a / b) * y1;
    return g;
}

// 模逆：ax ≡ 1 (mod m)，要求 gcd(a,m)=1；不存在返回 0
ll modinv(ll a, ll m){
    ll x, y;
    ll g = exgcd(a, m, x, y);
    if(g != 1) return 0;
    x %= m; if(x < 0) x += m;
    return x;
}

// 线性筛结果：primes 素数表；phi 欧拉函数；mu 莫比乌斯函数；is_prime 标记
struct sieve_result{
    std::vector<int> primes;
    std::vector<int> phi;
    std::vector<int> mu;
    std::vector<bool> is_prime;
};

// 线性筛（欧拉筛）：O(n) 同时求出素数、欧拉函数、莫比乌斯函数，大小 n+1
sieve_result linear_sieve(int n){
    sieve_result res;
    if(n < 0) n = 0;
    res.phi.assign(n+1, 0);
    res.mu.assign(n+1, 0);
    res.is_prime.assign(n+1, true);
    res.phi[1] = 1; res.mu[1] = 1;
    res.is_prime[0] = res.is_prime[1] = false;
    for(int i = 2; i <= n; i++){
        if(res.is_prime[i]){
            res.primes.push_back(i);
            res.phi[i] = i - 1;
            res.mu[i] = -1;
        }
        for(size_t j = 0; j < res.primes.size() && (ll)i * res.primes[j] <= n; j++){
            int p = res.primes[j];
            res.is_prime[i * p] = false;
            if(i % p == 0){
                res.phi[i * p] = res.phi[i] * p;
                res.mu[i * p] = 0;
                break;
            } else {
                res.phi[i * p] = res.phi[i] * (p - 1);
                res.mu[i * p] = -res.mu[i];
            }
        }
    }
    return res;
}

// 素数判定（试除法，适合常规范围；大数改用 Miller-Rabin）
bool is_prime_ll(ll n){
    if(n < 2) return false;
    if(n < 4) return true;
    if(n % 2 == 0) return false;
    for(ll i = 3; i * i <= n; i += 2)
        if(n % i == 0) return false;
    return true;
}

// 组合数 C(n,k) mod mod（基于阶乘+逆元）；k<0 或 k>n 返回 0
ll C(ll n, ll k, ll mod){
    if(k < 0 || k > n) return 0;
    k = std::min(k, n - k);
    ll num = 1 % mod, den = 1 % mod;
    for(ll i = 1; i <= k; i++){
        num = (__int128)num * ((n - k + i) % mod) % mod;
        den = (__int128)den * (i % mod) % mod;
    }
    return num * modinv(den, mod) % mod;
}

// Lucas 定理：C(n,k) mod p（p 为素数），大组合数取模
ll lucas(ll n, ll k, ll p){
    if(k == 0) return 1;
    return (C(n % p, k % p, p) * lucas(n / p, k / p, p)) % p;
}

// 离散对数 BSGS：求最小非负 x 使 a^x ≡ b (mod m)，要求 gcd(a,m)=1；无解返回 -1
ll bsgs(ll a, ll b, ll m){
    a %= m; b %= m;
    if(b == 1) return 0;
    ll n = (ll)std::sqrt(m) + 1;
    std::unordered_map<ll, ll> tb;
    ll e = 1;
    for(ll j = 0; j < n; j++){ if(!tb.count(e)) tb[e] = j; e = (__int128)e * a % m; }
    ll ainv = modinv(a, m);
    if(ainv == 0) return -1;
    ll cur = b;
    for(ll i = 0; i <= n; i++){
        if(tb.count(cur)) return i * n + tb[cur];
        cur = (__int128)cur * ainv % m;
    }
    return -1;
}

// 模奇素数 p 的一个原根；p<=1 返回 -1
ll primitive_root(ll p){
    if(p <= 1) return -1;
    if(p == 2) return 1;
    std::vector<ll> fac;
    ll x = p - 1;
    for(ll i = 2; i * i <= x; i++) if(x % i == 0){ fac.push_back(i); while(x % i == 0) x /= i; }
    if(x > 1) fac.push_back(x);
    for(ll g = 2; g < p; g++){
        bool ok = true;
        for(ll q : fac){ if(modpow(g, (p - 1) / q, p) == 1){ ok = false; break; } }
        if(ok) return g;
    }
    return -1;
}

} // namespace better_std
