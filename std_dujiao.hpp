#ifndef BETTER_STD_STD_DUJIAO_HPP
#define BETTER_STD_STD_DUJIAO_HPP

//===== better_std 杜教筛（数论函数前缀和）=====
//   基于线性筛预处理前 N 项 + 记忆化整除分块，O(n^(2/3)) 求：
//     M(n)=Σ_{i<=n} μ(i)，Phi(n)=Σ_{i<=n} φ(i)。
//   n 可为 long long（<=9e18，注意中间结果用 __int128/compat 防溢出由调用方保证范围）。
#include<vector>
#include<unordered_map>
#include<cstdint>
namespace better_std{

class dujiao{
    enum { LIM = 5000000 };
    std::vector<int> mu_, phi_, pre_mu_, pre_phi_;
    std::vector<long long> primes_;
    std::vector<char> is_comp_;
    int lim_;
    std::unordered_map<long long,long long> memo_mu_, memo_phi_;

    void sieve(){
        mu_.assign(lim_+1,0); phi_.assign(lim_+1,0);
        pre_mu_.assign(lim_+1,0); pre_phi_.assign(lim_+1,0);
        is_comp_.assign(lim_+1,0);
        mu_[1]=1; phi_[1]=1;
        for(int i=2;i<=lim_;i++){
            if(!is_comp_[i]){ primes_.push_back(i); mu_[i]=-1; phi_[i]=i-1; }
            for(size_t j=0;j<primes_.size() && (long long)i*primes_[j]<=lim_;j++){
                int p=primes_[j]; is_comp_[i*p]=1;
                if(i%p==0){ mu_[i*p]=0; phi_[i*p]=phi_[i]*p; break; }
                mu_[i*p]=-mu_[i]; phi_[i*p]=phi_[i]*(p-1);
            }
        }
        for(int i=1;i<=lim_;i++){ pre_mu_[i]=pre_mu_[i-1]+mu_[i]; pre_phi_[i]=pre_phi_[i-1]+phi_[i]; }
    }
    // Σ_{i<=n} μ(i)：Σ_{i<=n} M(⌊n/i⌋) = 1  ⇒ M(n)=1-Σ_{i=2..n}(r-l+1)M(⌊n/i⌋)
    long long calc_mu(long long n){
        if(n<=lim_) return pre_mu_[(int)n];
        auto it=memo_mu_.find(n); if(it!=memo_mu_.end()) return it->second;
        long long ans=1;
        for(long long l=2,r;l<=n;l=r+1){
            long long q=n/l; r=n/q;
            ans -= (r-l+1)*calc_mu(q);
        }
        memo_mu_[n]=ans; return ans;
    }
    // Σ_{i<=n} φ(i)：Σ_{d|n} φ(d)=n ⇒ Φ(n)=n(n+1)/2 - Σ_{i=2..n}(r-l+1)Φ(⌊n/i⌋)
    long long calc_phi(long long n){
        if(n<=lim_) return pre_phi_[(int)n];
        auto it=memo_phi_.find(n); if(it!=memo_phi_.end()) return it->second;
        long long ans=n*(n+1)/2;
        for(long long l=2,r;l<=n;l=r+1){
            long long q=n/l; r=n/q;
            ans -= (r-l+1)*calc_phi(q);
        }
        memo_phi_[n]=ans; return ans;
    }
public:
    explicit dujiao(int pre = LIM): lim_(std::min(pre, (int)LIM)){
        if(lim_<1) lim_=1;
        sieve();
    }
    long long mu_prefix(long long n){ if(n<1) return 0; return calc_mu(n); }
    long long phi_prefix(long long n){ if(n<1) return 0; return calc_phi(n); }
    // 单值 μ(n) / φ(n)（大 n 时用前缀差分：需 n-1 的缓存）
    int mu(long long n){ if(n<=lim_) return mu_[(int)n]; return (int)(calc_mu(n)-calc_mu(n-1)); }
    long long phi(long long n){ if(n<=lim_) return phi_[(int)n]; return calc_phi(n)-calc_phi(n-1); }
};

} // namespace better_std
#endif // BETTER_STD_STD_DUJIAO_HPP
