#pragma once
#include<iostream>
#include<vector>
#include<utility>
#include<algorithm>
namespace better_std{
    //r×c 矩阵，元素类型 T（int/long long/double/High_precision_number 皆可）
    //乘法需要 T 支持 +=/*，行列式需要 / 与 abs（ADL 或 std::abs）
    template<typename T>
    class matrix{
        private:
            int r_,c_;
            std::vector<std::vector<T>> a_;
        public:
            matrix():r_(0),c_(0){}
            matrix(int r,int c,const T& v=T()):r_(r),c_(c),a_(r,std::vector<T>(c,v)){}
            matrix(const std::vector<std::vector<T>>& a):r_((int)a.size()),c_(a.empty()?0:(int)a[0].size()),a_(a){}
            int rows() const{ return r_; }
            int cols() const{ return c_; }
            T& operator()(int i,int j){ return a_[i][j]; }
            T operator()(int i,int j) const{ return a_[i][j]; }

            matrix operator+(const matrix& o) const{
                matrix res(r_,c_);
                for(int i=0;i<r_;i++) for(int j=0;j<c_;j++) res(i,j)=a_[i][j]+o.a_[i][j];
                return res;
            }
            matrix operator-(const matrix& o) const{
                matrix res(r_,c_);
                for(int i=0;i<r_;i++) for(int j=0;j<c_;j++) res(i,j)=a_[i][j]-o.a_[i][j];
                return res;
            }
            matrix operator*(const matrix& o) const{//r×c × c×o.c
                matrix res(r_,o.c_,T());
                for(int i=0;i<r_;i++)
                    for(int k=0;k<c_;k++)
                        for(int j=0;j<o.c_;j++)
                            res(i,j)+=a_[i][k]*o.a_[k][j];
                return res;
            }
            matrix operator*(const T& s) const{//数乘
                matrix res(r_,c_);
                for(int i=0;i<r_;i++) for(int j=0;j<c_;j++) res(i,j)=a_[i][j]*s;
                return res;
            }
            matrix transpose() const{
                matrix t(c_,r_);
                for(int i=0;i<r_;i++) for(int j=0;j<c_;j++) t(j,i)=a_[i][j];
                return t;
            }
            bool operator==(const matrix& o) const{ return r_==o.r_ && c_==o.c_ && a_==o.a_; }
            bool operator!=(const matrix& o) const{ return !(*this==o); }
            T trace() const{
                T t=T();
                for(int i=0;i<r_&&i<c_;i++) t+=a_[i][i];
                return t;
            }
            //逆矩阵（高斯-约当；T 需支持除法，int 请勿用于非整除矩阵）。
            //方阵且可逆时返回逆矩阵，否则返回 0×0 空矩阵
            matrix inverse() const{
                int n=r_;
                if(n!=c_) return matrix();
                matrix m(n,2*n,T());
                for(int i=0;i<n;i++){
                    for(int j=0;j<n;j++) m(i,j)=a_[i][j];
                    m(i,n+i)=T(1);
                }
                using std::abs;
                for(int i=0;i<n;i++){
                    int p=i;
                    for(int j=i+1;j<n;j++) if(abs(m(j,i))>abs(m(p,i))) p=j;
                    if(m(p,i)==T(0)) return matrix();
                    if(p!=i) for(int j=0;j<2*n;j++) std::swap(m(i,j),m(p,j));
                    T d=m(i,i);
                    for(int j=0;j<2*n;j++) m(i,j)=m(i,j)/d;
                    for(int j=0;j<n;j++) if(j!=i){
                        T f=m(j,i);
                        for(int k=0;k<2*n;k++) m(j,k)=m(j,k)-f*m(i,k);
                    }
                }
                matrix inv(n,n);
                for(int i=0;i<n;i++) for(int j=0;j<n;j++) inv(i,j)=m(i,n+j);
                return inv;
            }
            static matrix identity(int n){
                matrix I(n,n,T());
                for(int i=0;i<n;i++) I(i,i)=T(1);
                return I;
            }
            //方阵快速幂
            matrix pow(long long k) const{
                matrix r=identity(r_), b=*this;
                while(k>0){
                    if(k&1) r=r*b;
                    b=b*b;
                    k>>=1;
                }
                return r;
            }
            //行列式（Bareiss 算法：仅整数乘除，对 int/High_precision_number 都精确）
            T det() const{
                using std::abs;//int/double 用 std::abs，High_precision_number 走 ADL
                if(r_==0) return T(1);
                matrix m=*this;
                int n=r_;
                T sign=T(1), prev=T(1);
                for(int k=0;k<n-1;k++){
                    int p=k;
                    for(int i=k+1;i<n;i++) if(abs(m(i,k))>abs(m(p,k))) p=i;
                    if(m(p,k)==T(0)) return T(0);
                    if(p!=k){ std::swap(m.a_[k],m.a_[p]); sign=T(0)-sign; }
                    T pivot=m(k,k);
                    for(int i=k+1;i<n;i++)
                        for(int j=k+1;j<n;j++)
                            m(i,j)=(m(i,j)*pivot - m(i,k)*m(k,j))/prev;
                    prev=pivot;
                }
                return sign*m(n-1,n-1);
            }
            void print() const{
                for(int i=0;i<r_;i++){
                    for(int j=0;j<c_;j++){
                        if(j) std::cout<<" ";
                        std::cout<<a_[i][j];
                    }
                    std::cout<<"\n";
                }
            }
    };
}
