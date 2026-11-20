#ifndef BETTER_STD_STD_COMBINATORICS_HPP
#define BETTER_STD_STD_COMBINATORICS_HPP

#include<vector>
#include<algorithm>
#include<climits>
namespace better_std{
    //===== 组合计数全家桶（模素数 MOD，要求 n < MOD）=====
    //  C / 卡特兰 / 第一·第二类斯特林数 / 贝尔数 / 整数划分数 / 错排 / 容斥并集
    template<int MOD=998244353>
    struct combinatorics{
        using ll=long long;
        static ll modpow(ll a,ll e){
            a%=MOD; if(a<0)a+=MOD;
            ll r=1;
            while(e){ if(e&1) r=r*a%MOD; a=a*a%MOD; e>>=1; }
            return r;
        }
        static ll inv(ll a){ return modpow(a,MOD-2); }
        //C(n,k)：O(min(k,n-k) log MOD)，n<MOD
        static ll C(int n,int k){
            if(n<0||k<0||k>n) return 0;
            k=std::min(k,n-k);
            ll r=1;
            for(int i=1;i<=k;i++) r=r*((ll)n-k+i)%MOD*inv(i)%MOD;
            return r;
        }
        static ll fact(int n){ ll r=1; for(int i=2;i<=n;i++) r=r*i%MOD; return r; }
        //卡特兰数 H(n)=C(2n,n)/(n+1)
        static ll catalan(int n){
            if(n<0) return 0;
            return C(2*n,n)*inv(n+1)%MOD;
        }
        //错排 D(n)=(n-1)(D(n-1)+D(n-2))
        static ll derangement(int n){
            if(n<0) return 0;
            if(n==0) return 1;
            if(n==1) return 0;
            ll d0=1,d1=0;
            for(int i=2;i<=n;i++){ ll d=(ll)(i-1)*(d0+d1)%MOD; d0=d1; d1=d; }
            return d1;
        }
        //第二类斯特林数 S(n,k)（n 个有标号球放 k 个无标号盒）
        static ll stirling2(int n,int k){
            if(n<0||k<0||k>n) return 0;
            std::vector<ll> dp(k+1,0); dp[0]=1;
            for(int i=1;i<=n;i++){
                for(int j=std::min(i,k);j>=1;j--)
                    dp[j]=((ll)j*dp[j]+dp[j-1])%MOD;
                dp[0]=0;  //S(i,0)=0（i>=1），滚动数组须显式清零
            }
            return dp[k];
        }
        //第一类斯特林数（无符号）s(n,k)（n 个元素的排列分成 k 个轮换）
        static ll stirling1(int n,int k){
            if(n<0||k<0||k>n) return 0;
            std::vector<ll> dp(k+1,0); dp[0]=1;
            for(int i=1;i<=n;i++){
                for(int j=std::min(i,k);j>=1;j--)
                    dp[j]=((ll)(i-1)*dp[j]+dp[j-1])%MOD;
                dp[0]=0;
            }
            return dp[k];
        }
        //贝尔数 B(n)=sum_k S(n,k)
        static ll bell(int n){
            if(n<0) return 0;
            ll r=0;
            for(int k=0;k<=n;k++){ r+=stirling2(n,k); if(r>=MOD) r-=MOD; }
            return r;
        }
        //整数划分数 p(n)（完全背包 O(n^2)）
        static ll partition_number(int n){
            if(n<0) return 0;
            std::vector<ll> dp(n+1,0); dp[0]=1;
            for(int i=1;i<=n;i++)
                for(int j=i;j<=n;j++){ dp[j]+=dp[j-i]; if(dp[j]>=MOD) dp[j]-=MOD; }
            return dp[n];
        }
        //容斥求 |A_1∪...∪A_n|：f(mask) 返回「满足 mask 中所有性质」的元素个数（模 MOD）
        template<class F>
        static ll union_size(int n,F f){
            if(n<0||n>=31) return 0;
            ll ans=0;
            for(int mask=1;mask<(1<<n);mask++){
                ll v=f(mask)%MOD; if(v<0) v+=MOD;
                int bits=0; for(int t=mask;t;t&=t-1) bits++;
                if(bits&1){ ans+=v; if(ans>=MOD) ans-=MOD; }
                else { ans-=v; if(ans<0) ans+=MOD; }
            }
            return ans;
        }
    };
}

#endif // BETTER_STD_STD_COMBINATORICS_HPP
