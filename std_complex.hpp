#pragma once
#include<iostream>
#include<cmath>
namespace better_std{
    //复数，元素类型 T（double / High_precision_floating_point_number<accuracy> 皆可）
    //超越函数经 ADL：double 走 std::，高精度类型走 better_std 的重载
    template<typename T=double>
    class std_complex{
        private:
            T re_,im_;
        public:
            std_complex():re_(T()),im_(T()){}
            std_complex(const T& r,const T& i=T()):re_(r),im_(i){}
            T real() const{ return re_; }
            T imag() const{ return im_; }
            void real(const T& r){ re_=r; }
            void imag(const T& i){ im_=i; }

            std_complex operator+(const std_complex& o) const{ return {re_+o.re_,im_+o.im_}; }
            std_complex operator-(const std_complex& o) const{ return {re_-o.re_,im_-o.im_}; }
            std_complex operator*(const std_complex& o) const{
                return {re_*o.re_-im_*o.im_, re_*o.im_+im_*o.re_};
            }
            std_complex operator/(const std_complex& o) const{
                T d=o.re_*o.re_+o.im_*o.im_;
                return {(re_*o.re_+im_*o.im_)/d, (im_*o.re_-re_*o.im_)/d};
            }
            std_complex operator-() const{ return {-re_,-im_}; }
            bool operator==(const std_complex& o) const{ return re_==o.re_&&im_==o.im_; }
            bool operator!=(const std_complex& o) const{ return !(*this==o); }

            std_complex conj() const{ return {re_,-im_}; }
            T norm() const{ return re_*re_+im_*im_; }
            T abs() const{
                using std::sqrt;
                return sqrt(norm());
            }
            T arg() const{
                using std::atan2;
                return atan2(im_,re_);
            }
            static std_complex polar(const T& r,const T& theta){
                using std::cos;
                using std::sin;
                return {r*cos(theta), r*sin(theta)};
            }
            //欧拉公式系
            std_complex exp() const{
                using std::exp;
                using std::cos;
                using std::sin;
                T e=exp(re_);
                return {e*cos(im_), e*sin(im_)};
            }
            std_complex log() const{
                using std::log;
                return {log(abs()), arg()};
            }
            std_complex sin() const{
                using std::sin;
                using std::cos;
                using std::sinh;
                using std::cosh;
                return {sin(re_)*cosh(im_), cos(re_)*sinh(im_)};
            }
            std_complex cos() const{
                using std::sin;
                using std::cos;
                using std::sinh;
                using std::cosh;
                return {cos(re_)*cosh(im_), -sin(re_)*sinh(im_)};
            }
            std_complex tan() const{ return sin()/cos(); }
            //sqrt：主分支（实部>=0；虚部符号跟随 im_）
            std_complex sqrt() const{
                using std::sqrt;
                T a=abs();
                T r=sqrt((a+re_)/T(2));
                T i=sqrt((a-re_)/T(2));
                if(im_<T(0)) i=T(0)-i;
                return {r,i};
            }
            //整数次幂（快速幂）
            std_complex pow(long long n) const{
                std_complex r(T(1),T(0)), b=*this;
                if(n<0) return (b.pow(-n)).reciprocal();
                while(n>0){
                    if(n&1) r=r*b;
                    b=b*b;
                    n>>=1;
                }
                return r;
            }
            std_complex reciprocal() const{
                T d=norm();
                return {re_/d, -im_/d};
            }
            //一般次幂 c^p = exp(p*log(c))
            std_complex pow(const std_complex& p) const{ return (p*log()).exp(); }

            friend std::ostream& operator<<(std::ostream& os,const std_complex& x){
                os<<x.re_;
                if(x.im_>=T(0)) os<<"+";
                os<<x.im_<<"i";
                return os;
            }
    };
}
