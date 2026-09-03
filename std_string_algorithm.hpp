#pragma once
#include<string>
#include<vector>
#include<utility>
#include<algorithm>
namespace better_std{
    //===== 字符串算法包：KMP / Z 函数 / 双模滚动哈希 =====

    //KMP 前缀函数 pi[i] = s[0..i] 的最长真前缀=真后缀长度
    inline std::vector<int> prefix_function(const std::string& s){
        int n=(int)s.size();
        std::vector<int> pi(n);
        for(int i=1;i<n;i++){
            int j=pi[i-1];
            while(j>0 && s[i]!=s[j]) j=pi[j-1];
            if(s[i]==s[j]) j++;
            pi[i]=j;
        }
        return pi;
    }
    //pat 在 text 中所有匹配的起始位置（升序）
    inline std::vector<int> kmp_search(const std::string& text,const std::string& pat){
        std::vector<int> res;
        if(pat.empty()||pat.size()>text.size()) return res;
        std::vector<int> pi=prefix_function(pat);
        int j=0;
        for(int i=0;i<(int)text.size();i++){
            while(j>0 && text[i]!=pat[j]) j=pi[j-1];
            if(text[i]==pat[j]) j++;
            if(j==(int)pat.size()){
                res.push_back(i-(int)pat.size()+1);
                j=pi[j-1];
            }
        }
        return res;
    }
    //Z 函数 z[i] = s 与 s[i..] 的最长公共前缀长度
    inline std::vector<int> z_function(const std::string& s){
        int n=(int)s.size();
        std::vector<int> z(n);
        int l=0,r=0;
        for(int i=1;i<n;i++){
            if(i<=r) z[i]=std::min(r-i+1,z[i-l]);
            while(i+z[i]<n && s[z[i]]==s[i+z[i]]) z[i]++;
            if(i+z[i]-1>r){ l=i; r=i+z[i]-1; }
        }
        return z;
    }
    //用 Z 函数查找 pat 在 text 中所有匹配起始位置
    inline std::vector<int> z_search(const std::string& text,const std::string& pat){
        std::vector<int> res;
        if(pat.empty()||pat.size()>text.size()) return res;
        std::string s=pat+"#"+text;
        std::vector<int> z=z_function(s);
        int base=(int)pat.size()+1;
        for(int i=base;i<(int)s.size();i++)
            if(z[i]>=(int)pat.size()) res.push_back(i-base);
        return res;
    }
    //双模滚动哈希（冲突概率极低）
    class std_string_hash{
        private:
            static const long long MOD1=1000000007;
            static const long long MOD2=1000000009;
            static const long long BASE=911382323;
            std::vector<long long> h1_,h2_,p1_,p2_;
        public:
            std_string_hash(const std::string& s){
                int n=(int)s.size();
                h1_.assign(n+1,0);h2_.assign(n+1,0);
                p1_.assign(n+1,1);p2_.assign(n+1,1);
                for(int i=0;i<n;i++){
                    h1_[i+1]=(h1_[i]*BASE+(long long)s[i])%MOD1;
                    h2_[i+1]=(h2_[i]*BASE+(long long)s[i])%MOD2;
                    p1_[i+1]=p1_[i]*BASE%MOD1;
                    p2_[i+1]=p2_[i]*BASE%MOD2;
                }
            }
            //子串 [l,r) 的哈希
            std::pair<long long,long long> get(int l,int r) const{
                long long x1=(h1_[r]-h1_[l]*p1_[r-l]%MOD1+MOD1)%MOD1;
                long long x2=(h2_[r]-h2_[l]*p2_[r-l]%MOD2+MOD2)%MOD2;
                return {x1,x2};
            }
            bool equal(int l1,int r1,int l2,int r2) const{ return get(l1,r1)==get(l2,r2); }
    };

    //===== Manacher 回文 =====
    //d1[i]：以 i 为中心的奇回文半径（含自身）；d2[i]：以 (i,i+1) 为中心的偶回文半径
    inline void manacher(const std::string& s,std::vector<int>& d1,std::vector<int>& d2){
        int n=(int)s.size();
        d1.assign(n,0);
        for(int i=0,l=0,r=-1;i<n;i++){
            int k=(i>r)?1:std::min(d1[l+r-i],r-i+1);
            while(i-k>=0&&i+k<n&&s[i-k]==s[i+k]) k++;
            d1[i]=k--;
            if(i+k>r){ l=i-k; r=i+k; }
        }
        d2.assign(n,0);
        for(int i=0,l=0,r=-1;i<n;i++){
            int k=(i>r)?0:std::min(d2[l+r-i+1],r-i+1);
            while(i-k-1>=0&&i+k<n&&s[i-k-1]==s[i+k]) k++;
            d2[i]=k--;
            if(i+k>r){ l=i-k-1; r=i+k; }
        }
    }
    //最长回文子串的区间 [l,r)
    inline std::pair<int,int> longest_palindrome(const std::string& s){
        std::vector<int> d1,d2;
        manacher(s,d1,d2);
        int best=0,l=0,r=0;
        for(int i=0;i<(int)s.size();i++){
            if(2*d1[i]-1>best){ best=2*d1[i]-1; l=i-d1[i]+1; r=i+d1[i]; }
            if(2*d2[i]>best){ best=2*d2[i]; l=i-d2[i]; r=i+d2[i]; }
        }
        return {l,r};
    }
}
