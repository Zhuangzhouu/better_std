#pragma once
#include<iostream>
#include"std_goodint.hpp"
using namespace better_std;
namespace better_std{
    //accuracy 位十进制精度的高精度有理数（分数）
    //分子分母用 High_precision_number<accuracy>，始终保持最简、分母为正
    template<int accuracy>
    class fraction{
        private:
            High_precision_number<accuracy> num_,den_;
            void normalize(){
                High_precision_number<accuracy> zero(0);
                if(den_ < zero){ den_ = zero - den_; num_ = zero - num_; }//分母归正
                if(num_ == zero){ den_ = High_precision_number<accuracy>(1); return; }
                High_precision_number<accuracy> g = gcd(abs(num_),abs(den_));
                if(!(g == High_precision_number<accuracy>(1))){ num_ = num_ / g; den_ = den_ / g; }
            }
        public:
            fraction():num_(0),den_(1){}
            fraction(long long v):num_(v),den_(1){}
            fraction(int v):num_(v),den_(1){}
            fraction(const High_precision_number<accuracy>& n,
                     const High_precision_number<accuracy>& d = High_precision_number<accuracy>(1))
                :num_(n),den_(d){ normalize(); }
            High_precision_number<accuracy> numerator() const{ return num_; }
            High_precision_number<accuracy> denominator() const{ return den_; }

            fraction operator+(const fraction& o) const{
                return fraction(num_*o.den_ + o.num_*den_, den_*o.den_);
            }
            fraction operator-(const fraction& o) const{
                return fraction(num_*o.den_ - o.num_*den_, den_*o.den_);
            }
            fraction operator*(const fraction& o) const{
                return fraction(num_*o.num_, den_*o.den_);
            }
            fraction operator/(const fraction& o) const{
                return fraction(num_*o.den_, den_*o.num_);
            }
            fraction operator-() const{ return fraction(High_precision_number<accuracy>(0)-num_, den_); }
            fraction reciprocal() const{ return fraction(den_, num_); }
            fraction& operator+=(const fraction& o){ return *this=*this+o; }
            fraction& operator-=(const fraction& o){ return *this=*this-o; }
            fraction& operator*=(const fraction& o){ return *this=*this*o; }
            fraction& operator/=(const fraction& o){ return *this=*this/o; }
            //整数次幂（快速幂）
            fraction pow(long long n) const{
                fraction r(1), b=*this;
                if(n<0){ b=b.reciprocal(); n=-n; }
                while(n>0){
                    if(n&1) r=r*b;
                    b=b*b;
                    n>>=1;
                }
                return r;
            }
            //向 -inf 取整 / 向 +inf 取整（返回整数型分数）
            fraction floor() const{
                High_precision_number<accuracy> q=num_/den_, rem=num_%den_;
                if(num_<High_precision_number<accuracy>(0) && rem!=High_precision_number<accuracy>(0))
                    q=q-High_precision_number<accuracy>(1);
                return fraction(q);
            }
            fraction ceil() const{
                High_precision_number<accuracy> q=num_/den_, rem=num_%den_;
                if(num_>High_precision_number<accuracy>(0) && rem!=High_precision_number<accuracy>(0))
                    q=q+High_precision_number<accuracy>(1);
                return fraction(q);
            }

            bool operator==(const fraction& o) const{ return num_*o.den_ == o.num_*den_; }
            bool operator<(const fraction& o) const{ return num_*o.den_ < o.num_*den_; }//分母均为正
            bool operator>(const fraction& o) const{ return o < *this; }
            bool operator<=(const fraction& o) const{ return !(*this > o); }
            bool operator>=(const fraction& o) const{ return !(*this < o); }
            bool operator!=(const fraction& o) const{ return !(*this == o); }

            double to_double() const{ return num_.to_double() / den_.to_double(); }
            friend std::ostream& operator<<(std::ostream& os,const fraction& x){
                os<<x.num_;
                if(!(x.den_ == High_precision_number<accuracy>(1))) os<<"/"<<x.den_;
                return os;
            }
    };
    //供 matrix 选主元等通用算法使用（ADL）
    template<int accuracy>
    fraction<accuracy> abs(const fraction<accuracy>& x){
        return (x < fraction<accuracy>(0)) ? (-x) : x;
    }
}
