#ifndef BETTER_STD_STD_THREE_SUM_HPP
#define BETTER_STD_STD_THREE_SUM_HPP

//===== better_std "GPT 版" 高效 3SUM 家族 =====
//  经典算法：
//    two_sum       —— 排序 + 双指针，返回一组下标（0-based）
//    three_sum     —— 排序 + 双指针 O(n^2)，返回所有不重复三元组（升序）
//    three_sum_closest —— 最接近 target 的三元组及和
//    three_sum_count   —— 计数版（重复元素按组合数计，LeetCode 923 风格）
//    four_sum      —— 排序 + 双指针 O(n^3)，返回不重复四元组
//  "GPT 版" 位压加速：
//    three_sum_exists_bitset —— 值域 bitset 反转 + 位移 + 位与，O(n^2/64) 判定存在性。
//      思路：集合 S 偏移到 [0,M]，位向量 B[v]=1；反转 R[i]=B[M-i]；
//      对每个 x，需 y+z=target-x，等价于 B & shift(R, M-need) 非空，一次 64 位与顶掉内层枚举。
//      适合值域不大（M 约 <=1e6）时的超快存在性判定。
//  所有函数 C++11 兼容、函数 inline、std:: 限定避免高精度重载遮蔽。
#include<vector>
#include<array>
#include<algorithm>
#include<cstdint>
#include<limits>
#include<utility>
namespace better_std{

//—— 2SUM：返回一组下标 {i,j}（0-based），无解返回 {-1,-1} ——
template<typename T>
inline std::pair<int,int> two_sum(const std::vector<T>& a,T target){
    int n=(int)a.size();
    std::vector<std::pair<T,int>> v;
    v.reserve(n);
    for(int i=0;i<n;i++) v.push_back({a[i],i});
    std::sort(v.begin(),v.end());
    int l=0,r=n-1;
    while(l<r){
        T s=v[l].first+v[r].first;
        if(s==target) return {v[l].second,v[r].second};
        if(s<target) l++; else r--;
    }
    return {-1,-1};
}

//—— 3SUM：所有不重复三元组（升序排列，元组间字典序升序）——
template<typename T>
inline std::vector<std::array<T,3>> three_sum(std::vector<T> a,T target=T(0)){
    std::vector<std::array<T,3>> res;
    std::sort(a.begin(),a.end());
    int n=(int)a.size();
    for(int i=0;i+2<n;i++){
        if(i>0&&a[i]==a[i-1]) continue;          // 跳过重复首元素
        int l=i+1,r=n-1;
        while(l<r){
            T s=a[i]+a[l]+a[r];
            if(s==target){
                res.push_back({a[i],a[l],a[r]});
                while(l<r&&a[l]==a[l+1]) l++;
                while(l<r&&a[r]==a[r-1]) r--;
                l++; r--;
            } else if(s<target) l++;
            else r--;
        }
    }
    return res;
}

//—— 3SUM Closest：返回 {三元组, 实际和} ——
template<typename T>
inline std::pair<std::array<T,3>,T> three_sum_closest(std::vector<T> a,T target){
    std::sort(a.begin(),a.end());
    int n=(int)a.size();
    if(n<3) return {std::array<T,3>{T(0),T(0),T(0)},T(0)};
    std::array<T,3> best={a[0],a[1],a[2]};
    T bestsum=a[0]+a[1]+a[2];
    for(int i=0;i+2<n;i++){
        int l=i+1,r=n-1;
        while(l<r){
            T s=a[i]+a[l]+a[r];
            T d=s>target?s-target:target-s;
            T bd=bestsum>target?bestsum-target:target-bestsum;
            if(d<bd){ bestsum=s; best={a[i],a[l],a[r]}; }
            if(s<target) l++;
            else if(s>target) r--;
            else return {best,bestsum};           // 精确命中
        }
    }
    return {best,bestsum};
}

//—— 3SUM Count：计数版（下标不同即不同三元组，值重复按组合数计）——
template<typename T>
inline long long three_sum_count(std::vector<T> a,T target){
    std::sort(a.begin(),a.end());
    int n=(int)a.size();
    long long cnt=0;
    for(int i=0;i+2<n;i++){
        // 计数版不能跳过重复首元素：不同下标 i 对应不同三元组
        int l=i+1,r=n-1;
        while(l<r){
            T s=a[i]+a[l]+a[r];
            if(s==target){
                if(a[l]==a[r]) cnt+=(long long)(r-l+1)*(r-l)/2;
                else {
                    int lc=1,rc=1;
                    while(l+lc<=r&&a[l+lc]==a[l]) lc++;
                    while(r-rc>=l&&a[r-rc]==a[r]) rc++;
                    cnt+=(long long)lc*rc;
                    l+=lc-1; r-=rc-1;
                }
                l++; r--;
            } else if(s<target) l++;
            else r--;
        }
    }
    return cnt;
}

//—— 4SUM：所有不重复四元组 ——
template<typename T>
inline std::vector<std::array<T,4>> four_sum(std::vector<T> a,T target=T(0)){
    std::vector<std::array<T,4>> res;
    std::sort(a.begin(),a.end());
    int n=(int)a.size();
    for(int i=0;i+3<n;i++){
        if(i>0&&a[i]==a[i-1]) continue;
        for(int j=i+1;j+2<n;j++){
            if(j>i+1&&a[j]==a[j-1]) continue;
            int l=j+1,r=n-1;
            while(l<r){
                T s=a[i]+a[j]+a[l]+a[r];
                if(s==target){
                    res.push_back({a[i],a[j],a[l],a[r]});
                    while(l<r&&a[l]==a[l+1]) l++;
                    while(l<r&&a[r]==a[r-1]) r--;
                    l++; r--;
                } else if(s<target) l++;
                else r--;
            }
        }
    }
    return res;
}

//========== "GPT 版" 位压加速 3SUM 存在性判定 ==========
namespace three_sum_detail{
    // 简易动态位向量（手写，避免非标准 boost）
    struct bitvec{
        int nbits;
        std::vector<unsigned long long> w;
        bitvec():nbits(0){}
        explicit bitvec(int n):nbits(n),w(((n+63)>>6),0){}
        void set(int i){ if(i>=0&&i<nbits) w[i>>6]|=(1ULL<<(i&63)); }
        bool test(int i) const { return i>=0&&i<nbits&&((w[i>>6]>>(i&63))&1ULL); }
        bool and_any(const bitvec& o) const {
            size_t m=std::min(w.size(),o.w.size());
            for(size_t i=0;i<m;i++) if(w[i]&o.w[i]) return true;
            return false;
        }
    };
    // 右移 s：out[i]=in[i+s]
    inline bitvec shr(const bitvec& in,int s){
        bitvec out(in.nbits);
        if(s<=0) return in;
        for(int i=0;i+s<in.nbits;i++) if(in.test(i+s)) out.set(i);
        return out;
    }
    // 左移 s：out[i+s]=in[i]
    inline bitvec shl_(const bitvec& in,int s){
        bitvec out(in.nbits);
        if(s<=0) return in;
        for(int i=0;i<in.nbits;i++) if(in.test(i)&&i+s<in.nbits) out.set(i+s);
        return out;
    }
}

// 位压 3SUM 存在性：存在 i,j,k（可相同下标按集合语义，需不同值请自行去重）使 a[i]+a[j]+a[k]=target。
// 值域跨度 M=max-min 建议 <= 1e6；复杂度 O(n * M/64)。
template<typename T>
inline bool three_sum_exists_bitset(const std::vector<T>& a,T target){
    if(a.size()<3) return false;
    T mn=*std::min_element(a.begin(),a.end());
    T mx=*std::max_element(a.begin(),a.end());
    long long span=(long long)mx-(long long)mn;
    if(span<0||span>8000000LL) return false;   // 值域过大则放弃（调用方改用 three_sum）
    int M=(int)span;
    int bits=M+1;
    three_sum_detail::bitvec B(bits);
    for(const T& v:a) B.set((int)((long long)v-mn));
    // 反转 R[i]=B[M-i]
    three_sum_detail::bitvec R(bits);
    for(int i=0;i<=M;i++) if(B.test(i)) R.set(M-i);
    // 对每个 x：需 (y- mn)+(z- mn) = target - x - 2*mn
    for(const T& x:a){
        long long need=(long long)target-(long long)x-2LL*(long long)mn;
        if(need<0||need>2LL*M) continue;
        // 判定 B & shift(R, M-need) 非空
        long long s=(long long)M-need;
        three_sum_detail::bitvec shifted(bits);
        if(s>=0) shifted=three_sum_detail::shr(R,(int)s);
        else     shifted=three_sum_detail::shl_(R,(int)(-s));
        if(B.and_any(shifted)) return true;
    }
    return false;
}

} // namespace better_std
#endif // BETTER_STD_STD_THREE_SUM_HPP
