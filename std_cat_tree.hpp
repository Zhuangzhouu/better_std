#ifndef BETTER_STD_STD_CAT_TREE_HPP
#define BETTER_STD_STD_CAT_TREE_HPP

#include<vector>
#include<algorithm>
#include<functional>
namespace better_std{
    //===== 猫树（Disjoint Sparse Table）=====
    //  O(n log n) 预处理、O(1) 区间查询，只需运算满足结合律（不要求幂等/可逆）。
    //  相比稀疏表（仅幂等运算 min/max/gcd），猫树支持区间和、乘积、矩阵乘、字符串拼接等。
    //  原理：第 k 层把数组分成长度 2^(k+1) 的块，对每块中点分别向左右做后缀/前缀聚合；
    //  查询 [l,r] 时取 k=floor(log2(l^r))，答案 = op(t[k][l], t[k][r])。
    template<typename T,class Op=std::plus<T>>
    class cat_tree{
        std::vector<T> a_;
        std::vector<std::vector<T>> t_;   //t_[k][i]
        std::vector<int> lg_;             //lg_[x]=floor(log2 x)
        int n_=0,K_=0;
    public:
        cat_tree()=default;
        explicit cat_tree(const std::vector<T>& a,Op op=Op{}){ build(a,op); }
        void build(const std::vector<T>& a,Op op=Op{}){
            a_=a; n_=(int)a_.size();
            K_=1;
            while((1<<K_)<std::max(1,n_)) K_++;
            t_.assign(K_,std::vector<T>(std::max(1,n_)));
            //l^r 是按位异或，最大可达 2^K_-1（可能远大于 n），lg 表须按 2^K_ 开
            lg_.assign(std::max(2,1<<K_),0);
            for(int i=2;i<(int)lg_.size();i++) lg_[i]=lg_[i/2]+1;
            for(int k=0;k<K_;k++){
                int half=1<<k, len=half<<1;
                for(int st=0;st<n_;st+=len){
                    int mid=std::min(st+half,n_), ed=std::min(st+len,n_);
                    if(mid>st){
                        t_[k][mid-1]=a_[mid-1];
                        for(int i=mid-2;i>=st;i--) t_[k][i]=op(a_[i],t_[k][i+1]);
                    }
                    if(mid<ed){
                        t_[k][mid]=a_[mid];
                        for(int i=mid+1;i<ed;i++) t_[k][i]=op(t_[k][i-1],a_[i]);
                    }
                }
            }
        }
        int size()const{ return n_; }
        //区间 [l,r] 闭区间聚合（0-based）；l==r 时直接返回单元素
        T query(int l,int r,Op op=Op{})const{
            if(l>r) std::swap(l,r);
            if(l==r) return a_[l];
            int k=lg_[l^r];
            return op(t_[k][l],t_[k][r]);
        }
    };
    template<typename T,class Op>
    using disjoint_sparse_table=cat_tree<T,Op>;
}

#endif // BETTER_STD_STD_CAT_TREE_HPP
