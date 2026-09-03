#pragma once
#include<iostream>
#include<vector>
namespace better_std{
    //树状数组（Binary Indexed Tree），1-based 下标
    //单点加、前缀和、区间和
    template<typename T=long long>
    class std_fenwick{
        private:
            int n_;
            std::vector<T> bit_;
        public:
            std_fenwick(int n=0):n_(n),bit_(n+1,T()){}
            //1-based 下标 i 处加 v
            void add(int i,const T& v){
                for(;i<=n_;i+=i&-i) bit_[i]+=v;
            }
            //1..i 前缀和
            T sum(int i) const{
                T s=T();
                for(;i>0;i-=i&-i) s+=bit_[i];
                return s;
            }
            //闭区间 [l,r] 和（1-based）
            T range_sum(int l,int r) const{ return sum(r)-sum(l-1); }
            //把下标 i 处的值设为 v（1-based）
            void set(int i,const T& v){
                T cur=sum(i)-sum(i-1);
                add(i,v-cur);
            }
            //第一个前缀和 >= k 的下标（1-based）；不存在返回 n_+1
            int kth(const T& k) const{
                int step=1;
                while(step*2<=n_) step<<=1;
                int idx=0;
                T acc=T();
                for(;step>0;step>>=1){
                    int nxt=idx+step;
                    if(nxt<=n_ && acc+bit_[nxt]<k){ idx=nxt; acc+=bit_[nxt]; }
                }
                return idx+1;
            }
            int size() const{ return n_; }
    };
}
