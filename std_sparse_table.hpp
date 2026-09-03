#pragma once
#include<vector>
#include<algorithm>
namespace better_std{
    //RMQ 稀疏表：O(n log n) 预处理，O(1) 区间最值查询
    //MAX=true 求最大值，默认求最小值
    template<typename T=long long,bool MAX=false>
    class std_sparse_table{
        private:
            int n_,k_;
            std::vector<std::vector<T>> st_;
            static T comb(const T& a,const T& b){
                return MAX ? (a>b?a:b) : (a<b?a:b);
            }
        public:
            std_sparse_table(){}
            std_sparse_table(const std::vector<T>& a){ build(a); }
            void build(const std::vector<T>& a){
                n_=(int)a.size();
                k_=0;
                while((1<<k_)<=n_) k_++;
                st_.assign(k_,std::vector<T>(n_));
                st_[0]=a;
                for(int k=1;k<k_;k++)
                    for(int i=0;i+(1<<k)<=n_;i++)
                        st_[k][i]=comb(st_[k-1][i],st_[k-1][i+(1<<(k-1))]);
            }
            //闭区间 [l,r] 最值
            T query(int l,int r) const{
                int k=31-__builtin_clz(r-l+1);
                return comb(st_[k][l],st_[k][r-(1<<k)+1]);
            }
            int size() const{ return n_; }
    };
}
