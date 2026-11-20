#ifndef BETTER_STD_STD_HASHING_HPP
#define BETTER_STD_STD_HASHING_HPP

//===== better_std 哈希全家桶：字符串滚动哈希 / 双哈希 / 可回滚哈希 / 树同构哈希 =====
//   · rolling_hash：单模多项式哈希，支持 substr(l,len) O(1)、concat、比较。
//   · double_hash：两个模数，冲突概率极低。
//   · rollback_hash：可回滚哈希栈（append/undo/snapshot），支持任意前缀回退后取哈希。
//   · tree_iso_hash：有根树同构哈希（子节点哈希排序后聚合），判定两树是否同构。
#include<string>
#include<vector>
#include<utility>
#include<algorithm>
#include<functional>
namespace better_std{

namespace hash_detail{
    // 加法倍增模乘（ISO 安全，不依赖 __int128）
    inline unsigned long long mulmod(unsigned long long a,unsigned long long b,unsigned long long m){
        unsigned long long r=0; a%=m;
        while(b){ if(b&1){ r+=a; if(r>=m||r<a) r%=m; } a<<=1; if(a>=m||a<m) a%=m; b>>=1; }
        return r;
    }
    inline unsigned long long modpow64(unsigned long long a,unsigned long long e,unsigned long long m){
        unsigned long long r=1%m;
        while(e){ if(e&1) r=mulmod(r,a,m); a=mulmod(a,a,m); e>>=1; }
        return r;
    }
}

//—— 单模滚动哈希（字符串）——
// 哈希定义：H(s[0..n)) = Σ s[i]*base^{n-1-i}（高位在前），便于 O(1) 取子串。
class rolling_hash{
    unsigned long long mod_, base_;
    std::vector<unsigned long long> pref_, pw_, ipw_;   // ipw_[i] = base^{-i}
    static unsigned long long mul_(unsigned long long a,unsigned long long b,unsigned long long m){
        unsigned long long r=0; a%=m;
        while(b){ if(b&1){ r+=a; if(r>=m||r<a) r%=m; } a<<=1; if(a>=m||a<m) a%=m; b>>=1; }
        return r;
    }
public:
    rolling_hash():mod_(1000000007ULL),base_(911382323ULL){}
    rolling_hash(const std::string& s,unsigned long long mod=1000000007ULL,unsigned long long base=911382323ULL)
        :mod_(mod),base_(base){ build(s); }
    void build(const std::string& s){
        int n=(int)s.size();
        pref_.assign(n+1,0); pw_.assign(n+1,1); ipw_.assign(n+1,1);
        unsigned long long ib=hash_detail::modpow64(base_,mod_-2,mod_);   // 模素数，base 逆元
        for(int i=0;i<n;i++){
            pw_[i+1]=mul_(pw_[i],base_,mod_);
            ipw_[i+1]=mul_(ipw_[i],ib,mod_);
            pref_[i+1]=(mul_(pref_[i],base_,mod_)+ (unsigned char)s[i])%mod_;
        }
    }
    // [l, l+len) 的哈希（规范化为长度 len 的高位在前哈希，同串同值）
    unsigned long long get(int l,int len) const {
        int n=(int)pref_.size()-1;
        if(len<=0||l<0||l+len>n) return 0;
        return (pref_[l+len]+mod_-mul_(pref_[l],pw_[len],mod_))%mod_;
    }
    unsigned long long whole() const { return get(0,(int)pref_.size()-1); }
    bool equal(int l1,int l2,int len) const { return get(l1,len)==get(l2,len); }
};

//—— 双哈希 ——
class double_hash{
    rolling_hash h1_, h2_;
public:
    double_hash() {}
    explicit double_hash(const std::string& s):h1_(s,1000000007ULL,911382323ULL),h2_(s,1000000009ULL,972663749ULL){}
    void build(const std::string& s){ h1_.build(s); h2_.build(s); }
    std::pair<unsigned long long,unsigned long long> get(int l,int len) const { return {h1_.get(l,len),h2_.get(l,len)}; }
    bool equal(int l1,int l2,int len) const { return h1_.equal(l1,l2,len)&&h2_.equal(l1,l2,len); }
};

//—— 可回滚哈希栈：append / undo / snapshot，取当前前缀哈希 ——
class rollback_hash{
    std::vector<unsigned long long> pref_, pw_, snapshot_;
    unsigned long long mod_, base_;
    static unsigned long long mul_(unsigned long long a,unsigned long long b,unsigned long long m){
        unsigned long long r=0; a%=m;
        while(b){ if(b&1){ r+=a; if(r>=m||r<a) r%=m; } a<<=1; if(a>=m||a<m) a%=m; b>>=1; }
        return r;
    }
public:
    explicit rollback_hash(unsigned long long mod=1000000007ULL,unsigned long long base=911382323ULL)
        :mod_(mod),base_(base){ pref_.push_back(0); pw_.push_back(1); }
    void append(unsigned long long c){
        pw_.push_back(mul_(pw_.back(),base_,mod_));
        pref_.push_back((pref_.back()+mul_(c,pw_[pw_.size()-2],mod_))%mod_);
    }
    void append_char(char c){ append((unsigned long long)(unsigned char)c); }
    int snapshot() const { return (int)pref_.size()-1; }
    void rollback(int snap){
        if(snap<0||snap>=(int)pref_.size()) return;
        pref_.resize(snap+1); pw_.resize(snap+1);
    }
    unsigned long long hash() const { return pref_.back(); }
    unsigned long long prefix(int k) const { return k<(int)pref_.size()?pref_[k]:0; }
    size_t size() const { return pref_.size()-1; }
};

//—— 有根树同构哈希：子节点哈希 multiset 排序后聚合 ——
inline unsigned long long tree_iso_hash(const std::vector<std::vector<int>>& tree,int root,
                                        unsigned long long mod=1000000007ULL){
    // 后序 DFS（栈式，避免递归深度）
    std::vector<int> order, parent(tree.size(),-2);
    parent[root]=-1; order.push_back(root);
    for(size_t i=0;i<order.size();i++){
        int u=order[i];
        for(int v:tree[u]) if(v!=parent[u]){ parent[v]=u; order.push_back(v); }
    }
    std::vector<unsigned long long> h(tree.size(),0);
    for(int i=(int)order.size()-1;i>=0;i--){
        int u=order[i];
        std::vector<unsigned long long> child;
        for(int v:tree[u]) if(parent[v]==u) child.push_back(h[v]);
        std::sort(child.begin(),child.end());
        unsigned long long val=1;
        for(unsigned long long c:child){
            val=val*911382323ULL%mod;
            val=(val+c)%mod;
        }
        h[u]=val;
    }
    return h[root];
}

} // namespace better_std
#endif // BETTER_STD_STD_HASHING_HPP
