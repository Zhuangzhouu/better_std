#pragma once
#include<iostream>
#include<string>
#include<cmath>
#include<cstdio>
#include<limits>
#include"std_goodint.hpp"
using namespace better_std;
namespace better_std{
    template<int accuracy>
    class High_precision_floating_point_number{
        private:
            High_precision_number<accuracy*4> hpn;//(accuracy*4)bits number
            int div_number;
            //10^n，作为内部 High_precision_number<accuracy*4> 返回
            High_precision_number<accuracy*4> pow10(int n) const{
                High_precision_number<accuracy*4> r(1);
                for(int i=0;i<n;i++) r=r*High_precision_number<accuracy*4>(10);
                return r;
            }
        public:
            int get_accuracy() const{
                return div_number;
            }
            High_precision_number<accuracy*4> mantissa() const{
                return hpn;
            }
            //把值四舍五入到 n 位小数（用于约束迭代过程中的 div_number 增长）
            High_precision_floating_point_number to_decimals(int n) const{
                if(n < 0) n = 0;
                int d = div_number;
                if(d <= n) return *this;
                int drop = d - n;
                High_precision_number<accuracy*4> H = hpn;
                High_precision_number<accuracy*4> p = pow10(drop);
                auto qr = H.divmod(p);
                High_precision_number<accuracy*4> q = qr.first;
                High_precision_number<accuracy*4> r = qr.second;
                High_precision_number<accuracy*4> two(2);
                if(!(r * two < p)) q = q + High_precision_number<accuracy*4>(1);
                return High_precision_floating_point_number<accuracy>(q, n);
            }
            High_precision_floating_point_number():hpn(0),div_number(0){}
            High_precision_floating_point_number(int x):hpn(x),div_number(0){}
            High_precision_floating_point_number(long long x):hpn(x),div_number(0){}
            template<int X>
            High_precision_floating_point_number(High_precision_number<X> v,int d):hpn(v),div_number(d){}
            //goodint → gooddouble 隐式提升（整数部分，div_number=0），使 gd+hpn / gd<hpn 等混合可用
            template<int X>
            High_precision_floating_point_number(const High_precision_number<X>& v):hpn(v),div_number(0){}
            //跨精度转换（不同 accuracy 特化之间互相构造，如 <30> → <50>）
            template<int> friend class High_precision_floating_point_number;
            template<int X>
            High_precision_floating_point_number(const High_precision_floating_point_number<X>& o):hpn(o.hpn),div_number(o.div_number){}
            High_precision_floating_point_number(double x,int dec=accuracy){
                //double 仅约 17 位有效数字，转成十进制串再交给 string 构造，避免 long long 溢出
                char buf[64];
                int prec=dec;
                if(prec<0)prec=0;
                if(prec>17)prec=17;
                std::snprintf(buf,sizeof(buf),"%.*f",prec,x);
                *this=High_precision_floating_point_number(std::string(buf));
            }
            High_precision_floating_point_number(const std::string& s){
                bool neg=false;size_t start=0;
                if(s.size()>0 && (s[0]=='-'||s[0]=='+')){neg=(s[0]=='-');start=1;}
                size_t dot=s.find('.',start);
                std::string intpart,fracpart;
                if(dot==std::string::npos){intpart=s.substr(start);fracpart="";}
                else{intpart=s.substr(start,dot-start);fracpart=s.substr(dot+1);}
                std::string combined=intpart+fracpart;
                if(combined.empty())combined="0";
                hpn=High_precision_number<accuracy*4>(combined);
                if(neg)hpn=High_precision_number<accuracy*4>(0)-hpn;
                div_number=(int)fracpart.size();
            }
            double to_double() const{
                return hpn.to_double()/std::pow(10.0,div_number);
            }
            High_precision_floating_point_number operator+() const{return *this;}
            High_precision_floating_point_number operator-() const{return High_precision_floating_point_number(-hpn,div_number);}
            High_precision_floating_point_number operator+(const High_precision_floating_point_number other) const{
                int d=std::max(div_number,other.div_number);
                High_precision_number<accuracy*4> a=hpn*pow10(d-div_number);
                High_precision_number<accuracy*4> b=other.hpn*pow10(d-other.div_number);
                return {a+b,d};
            }
            High_precision_floating_point_number operator-(const High_precision_floating_point_number other) const{
                int d=std::max(div_number,other.div_number);
                High_precision_number<accuracy*4> a=hpn*pow10(d-div_number);
                High_precision_number<accuracy*4> b=other.hpn*pow10(d-other.div_number);
                return {a-b,d};
            }
            High_precision_floating_point_number operator*(const High_precision_floating_point_number other) const{
                return {hpn*other.hpn,div_number+other.div_number};
            }
            High_precision_floating_point_number operator/(const High_precision_floating_point_number other) const{
                int M=std::max(div_number,other.div_number)+accuracy;
                int shift=M+other.div_number-div_number;//≥accuracy>0
                High_precision_number<accuracy*4> num=hpn*pow10(shift);
                std::pair<High_precision_number<accuracy*4>,High_precision_number<accuracy*4>> p=num.divmod(other.hpn);
                High_precision_number<accuracy*4> q=p.first;
                High_precision_number<accuracy*4> rem=p.second;
                High_precision_number<accuracy*4> two_rem=rem*High_precision_number<accuracy*4>(2);
                if(!(two_rem<other.hpn)) q=q+High_precision_number<accuracy*4>(1);//四舍五入
                return {q,M};
            }
            //===== 复合赋值（内置类型参数经非 explicit 构造隐式转换）=====
            High_precision_floating_point_number& operator+=(const High_precision_floating_point_number& o){*this=*this+o;return *this;}
            High_precision_floating_point_number& operator-=(const High_precision_floating_point_number& o){*this=*this-o;return *this;}
            High_precision_floating_point_number& operator*=(const High_precision_floating_point_number& o){*this=*this*o;return *this;}
            High_precision_floating_point_number& operator/=(const High_precision_floating_point_number& o){*this=*this/o;return *this;}
            bool operator==(const High_precision_floating_point_number& o) const{
                int d = div_number>o.div_number ? div_number : o.div_number;
                High_precision_number<accuracy*4> A = hpn * pow10(d-div_number);
                High_precision_number<accuracy*4> B = o.hpn * pow10(d-o.div_number);
                return A == B;
            }
            bool operator<(const High_precision_floating_point_number& o) const{
                int d = div_number>o.div_number ? div_number : o.div_number;
                High_precision_number<accuracy*4> A = hpn * pow10(d-div_number);
                High_precision_number<accuracy*4> B = o.hpn * pow10(d-o.div_number);
                return A < B;
            }
            bool operator>(const High_precision_floating_point_number& o) const{ return o < *this; }
            bool operator<=(const High_precision_floating_point_number& o) const{ return !(*this > o); }
            bool operator>=(const High_precision_floating_point_number& o) const{ return !(*this < o); }
            bool operator!=(const High_precision_floating_point_number& o) const{ return !(*this == o); }
            friend std::ostream& operator<<(std::ostream& os,const High_precision_floating_point_number& x){
                std::string num=x.hpn.to_string();
                bool neg=(num.size()>0&&num[0]=='-');
                if(neg)num=num.substr(1);
                int div=x.div_number;
                if(div>0){
                    if((int)num.size()<=div) num=std::string(div+1-(int)num.size(),'0')+num;
                    int dot=(int)num.size()-div;
                    num=num.substr(0,dot)+"."+num.substr(dot);
                }
                if(neg)os<<'-';
                os<<num;
                return os;
            }
    };

    //===== 标准库「double 输入」函数的重载（<cmath> 为主，兼 <algorithm> 的 min/max/clamp）=====
    //这些函数原本只在 namespace std 中以 double/long double 等内建浮点类型提供，
    //High_precision_floating_point_number 无法隐式匹配，故在 better_std 中提供重载，
    //借由 ADL（参数类型所在命名空间）即可被 unqualified 调用解析到。
    //注意：不能向 namespace std 添加重载（属未定义行为），故 std::sqrt(hpn) 不会被解析，
    //请用 sqrt(hpn) 或 better_std::sqrt(hpn)。
    //超越函数（exp/log/sin/cos/atan/pow…）在固定 accuracy 下用高精度常数(π,ln2,ln10)
    //与泰勒/牛顿/级数实现，结果精确到该类型自身的十进制位数（accuracy 位有效数字），
    //不会超过 double 的精度上限时与其在有效数字范围内一致。

    //—— 高精度常数（源自字符串，约 50 位小数，accuracy<=~50 时充分精确）——
    template<int accuracy>
    const High_precision_floating_point_number<accuracy>& _hp_pi(){
        static High_precision_floating_point_number<accuracy> v("3.14159265358979323846264338327950288419716939937510");
        return v;
    }
    template<int accuracy>
    const High_precision_floating_point_number<accuracy>& _hp_ln2(){
        static High_precision_floating_point_number<accuracy> v("0.69314718055994530941723212145817656807550013436025");
        return v;
    }
    template<int accuracy>
    const High_precision_floating_point_number<accuracy>& _hp_ln10(){
        static High_precision_floating_point_number<accuracy> v("2.30258509299404568401799145468436420760110148862877");
        return v;
    }

    //—— 内部工具 ——
    //整数次幂（指数 long long，精确）
    template<int accuracy>
    High_precision_floating_point_number<accuracy> _hp_pow_int(const High_precision_floating_point_number<accuracy>& a, long long n){
        using T = High_precision_floating_point_number<accuracy>;
        if(n==0) return T(1);
        if(n<0) return T(1) / _hp_pow_int(a, -n);
        T result(1), base = a;
        unsigned long long m = (unsigned long long)n;
        while(m>0){
            if(m & 1ull) result = result * base;
            base = base * base;
            m >>= 1;
        }
        return result;
    }
    //exp 级数（|r| 已较小，~0.35）
    template<int accuracy>
    High_precision_floating_point_number<accuracy> _hp_exp_series(const High_precision_floating_point_number<accuracy>& r){
        using T = High_precision_floating_point_number<accuracy>;
        T sum(1), term(1);
        for(int n=1;n<=200;n++){
            term = (term * r / T(n)).to_decimals(accuracy);
            sum = (sum + term).to_decimals(accuracy);
            if(term == T(0)) break;
        }
        return sum;
    }
    //log 级数（m in [1,2)）
    template<int accuracy>
    High_precision_floating_point_number<accuracy> _hp_log_series(const High_precision_floating_point_number<accuracy>& m){
        using T = High_precision_floating_point_number<accuracy>;
        T z = (m - T(1)) / (m + T(1));
        T z2 = z * z;
        T term = z, sum = z;
        for(int n=1;n<=200;n++){
            term = (term * z2).to_decimals(accuracy);
            sum = (sum + term / T(2*n+1)).to_decimals(accuracy);
            if(term == T(0)) break;
        }
        return (sum * T(2)).to_decimals(accuracy);
    }
    //sin 级数（r in [-pi,pi]）
    template<int accuracy>
    High_precision_floating_point_number<accuracy> _hp_sin_series(const High_precision_floating_point_number<accuracy>& r){
        using T = High_precision_floating_point_number<accuracy>;
        T r2 = r * r;
        T term = r, sum = r;
        for(int n=1;n<=200;n++){
            term = (term * r2 / T(2*n*(2*n+1))).to_decimals(accuracy);
            if(n%2==1) sum = (sum - term).to_decimals(accuracy);
            else sum = (sum + term).to_decimals(accuracy);
            if(term == T(0)) break;
        }
        return sum;
    }
    //cos 级数（r in [-pi,pi]）
    template<int accuracy>
    High_precision_floating_point_number<accuracy> _hp_cos_series(const High_precision_floating_point_number<accuracy>& r){
        using T = High_precision_floating_point_number<accuracy>;
        T r2 = r * r;
        T term(1), sum(1);
        for(int n=1;n<=200;n++){
            term = (term * r2 / T(2*n*(2*n-1))).to_decimals(accuracy);
            if(n%2==1) sum = (sum - term).to_decimals(accuracy);
            else sum = (sum + term).to_decimals(accuracy);
            if(term == T(0)) break;
        }
        return sum;
    }
    //atan 级数（|x| <= 0.5，收敛快）
    template<int accuracy>
    High_precision_floating_point_number<accuracy> _hp_atan_series(const High_precision_floating_point_number<accuracy>& x){
        using T = High_precision_floating_point_number<accuracy>;
        T x2 = x * x;
        T term = x, sum = x;
        for(int n=1;n<=300;n++){
            term = (term * x2).to_decimals(accuracy);
            if(n%2==1) sum = (sum - term / T(2*n+1)).to_decimals(accuracy);
            else sum = (sum + term / T(2*n+1)).to_decimals(accuracy);
            if(term == T(0)) break;
        }
        return sum;
    }

    //===== 结构型 / 取整型 =====
    template<int accuracy>
    High_precision_floating_point_number<accuracy> abs(const High_precision_floating_point_number<accuracy>& x){
        using T = High_precision_floating_point_number<accuracy>;
        T zero(0);
        return (x < zero) ? (T(0) - x) : x;
    }
    template<int accuracy>
    High_precision_floating_point_number<accuracy> fabs(const High_precision_floating_point_number<accuracy>& x){ return abs(x); }

    //借助内部 mantissa() 与 get_accuracy() 做取整；10^d 用字符串构造
    template<int accuracy>
    High_precision_floating_point_number<accuracy> trunc(const High_precision_floating_point_number<accuracy>& x){
        using T = High_precision_floating_point_number<accuracy>;
        using H = High_precision_number<accuracy*4>;
        int d = x.get_accuracy();
        H Hx = x.mantissa();
        H ten = H(std::string("1") + std::string(d, '0'));
        H q = Hx.divmod(ten).first; //向零取整
        return T(q, 0);
    }
    template<int accuracy>
    High_precision_floating_point_number<accuracy> floor(const High_precision_floating_point_number<accuracy>& x){
        using T = High_precision_floating_point_number<accuracy>;
        using H = High_precision_number<accuracy*4>;
        int d = x.get_accuracy();
        H Hx = x.mantissa();
        H ten = H(std::string("1") + std::string(d, '0'));
        auto p = Hx.divmod(ten);
        H q = p.first; H r = p.second; H zero(0);
        if(Hx < zero && !(r == zero)) q = q - H(1);
        return T(q, 0);
    }
    template<int accuracy>
    High_precision_floating_point_number<accuracy> ceil(const High_precision_floating_point_number<accuracy>& x){
        using T = High_precision_floating_point_number<accuracy>;
        using H = High_precision_number<accuracy*4>;
        int d = x.get_accuracy();
        H Hx = x.mantissa();
        H ten = H(std::string("1") + std::string(d, '0'));
        auto p = Hx.divmod(ten);
        H q = p.first; H r = p.second; H zero(0);
        if(Hx > zero && !(r == zero)) q = q + H(1);
        return T(q, 0);
    }
    template<int accuracy>
    High_precision_floating_point_number<accuracy> round(const High_precision_floating_point_number<accuracy>& x){
        using T = High_precision_floating_point_number<accuracy>;
        using H = High_precision_number<accuracy*4>;
        int d = x.get_accuracy();
        H Hx = x.mantissa();
        H ten = H(std::string("1") + std::string(d, '0'));
        auto p = Hx.divmod(ten);
        H q = p.first; H r = p.second; H zero(0);
        H ar = (r < zero) ? (zero - r) : r;
        if(ar * H(2) >= ten){ if(Hx < zero) q = q - H(1); else q = q + H(1); } //半值远离零
        return T(q, 0);
    }
    template<int accuracy>
    High_precision_floating_point_number<accuracy> fma(const High_precision_floating_point_number<accuracy>& a,
                                                       const High_precision_floating_point_number<accuracy>& b,
                                                       const High_precision_floating_point_number<accuracy>& c){
        return a * b + c;
    }
    template<int accuracy>
    High_precision_floating_point_number<accuracy> fmod(const High_precision_floating_point_number<accuracy>& a,
                                                         const High_precision_floating_point_number<accuracy>& b){
        using T = High_precision_floating_point_number<accuracy>;
        T q = trunc(a / b);
        return a - q * b;
    }
    template<int accuracy>
    High_precision_floating_point_number<accuracy> remainder(const High_precision_floating_point_number<accuracy>& a,
                                                             const High_precision_floating_point_number<accuracy>& b){
        using T = High_precision_floating_point_number<accuracy>;
        T q = round(a / b);
        return a - q * b;
    }
    template<int accuracy>
    High_precision_floating_point_number<accuracy> copysign(const High_precision_floating_point_number<accuracy>& a,
                                                             const High_precision_floating_point_number<accuracy>& b){
        using T = High_precision_floating_point_number<accuracy>;
        T az = abs(a);
        return (b < T(0)) ? (T(0) - az) : az;
    }
    template<int accuracy>
    High_precision_floating_point_number<accuracy> fmax(const High_precision_floating_point_number<accuracy>& a,
                                                         const High_precision_floating_point_number<accuracy>& b){
        return (a < b) ? b : a;
    }
    template<int accuracy>
    High_precision_floating_point_number<accuracy> fmin(const High_precision_floating_point_number<accuracy>& a,
                                                         const High_precision_floating_point_number<accuracy>& b){
        return (a < b) ? a : b;
    }
    template<int accuracy>
    High_precision_floating_point_number<accuracy> fdim(const High_precision_floating_point_number<accuracy>& a,
                                                         const High_precision_floating_point_number<accuracy>& b){
        using T = High_precision_floating_point_number<accuracy>;
        return (a > b) ? (a - b) : T(0);
    }
    template<int accuracy>
    bool signbit(const High_precision_floating_point_number<accuracy>& x){
        return x < High_precision_floating_point_number<accuracy>(0);
    }

    //===== 超越函数 =====
    template<int accuracy>
    High_precision_floating_point_number<accuracy> sqrt(const High_precision_floating_point_number<accuracy>& x){
        using T = High_precision_floating_point_number<accuracy>;
        T zero(0);
        if(!(x > zero)) return zero; //x<=0：0（x<0 在实数范围内无定义）
        double d = x.to_double();
        T g( (std::isfinite(d)) ? std::sqrt(d) : 1.0 );
        g = g.to_decimals(accuracy);
        for(int i=0;i<40;i++){
            T ng = ((g + x / g) * T(0.5)).to_decimals(accuracy);
            if(ng == g) break;
            g = ng;
        }
        return g;
    }
    template<int accuracy>
    High_precision_floating_point_number<accuracy> cbrt(const High_precision_floating_point_number<accuracy>& x){
        using T = High_precision_floating_point_number<accuracy>;
        T zero(0);
        double d = x.to_double();
        T g( (std::isfinite(d) && d!=0.0) ? std::cbrt(std::fabs(d)) : 1.0 );
        if(x < zero) g = T(0) - g;
        for(int i=0;i<60;i++){
            T ng = ((g*g*g*T(2) + x) / (g*g*T(3))).to_decimals(accuracy);
            if(ng == g) break;
            g = ng;
        }
        return g;
    }
    template<int accuracy>
    High_precision_floating_point_number<accuracy> exp(const High_precision_floating_point_number<accuracy>& x){
        using T = High_precision_floating_point_number<accuracy>;
        T ln2 = _hp_ln2<accuracy>();
        T r = x / ln2;
        long long k = std::llround(r.to_double());
        T rem = (x - T(k) * ln2).to_decimals(accuracy);
        T e = _hp_exp_series(rem);
        T pow2k = _hp_pow_int(T(2), k);
        return pow2k * e;
    }
    template<int accuracy>
    High_precision_floating_point_number<accuracy> log(const High_precision_floating_point_number<accuracy>& x){
        using T = High_precision_floating_point_number<accuracy>;
        T zero(0), one(1), two(2);
        if(!(x > zero)) return zero; //x<=0 无定义，返回 0
        T m = x; long long k = 0;
        while(m >= two){ m = (m / two).to_decimals(accuracy); k++; }
        while(m < one){ m = (m * two).to_decimals(accuracy); k--; }
        T lm = _hp_log_series(m);
        return T(k) * _hp_ln2<accuracy>() + lm;
    }
    template<int accuracy>
    High_precision_floating_point_number<accuracy> pow(const High_precision_floating_point_number<accuracy>& a,
                                                        const High_precision_floating_point_number<accuracy>& b){
        using T = High_precision_floating_point_number<accuracy>;
        T zero(0), one(1);
        if(b == zero) return one;
        T bf = trunc(b);
        if(b == bf){ //整数指数：精确
            long long n = std::llround(bf.to_double());
            T mag = _hp_pow_int(abs(a), std::llabs(n));
            if(n < 0) return one / mag;
            if(a < zero && (n % 2 != 0)) return T(0) - mag; //负底数奇次幂为负
            return mag;
        }
        if(a < zero) return zero; //负底数且非整数指数：实数范围内无定义
        return exp((b * log(a)).to_decimals(accuracy));
    }
    template<int accuracy>
    High_precision_floating_point_number<accuracy> exp2(const High_precision_floating_point_number<accuracy>& x){
        return exp(x * _hp_ln2<accuracy>());
    }
    template<int accuracy>
    High_precision_floating_point_number<accuracy> expm1(const High_precision_floating_point_number<accuracy>& x){
        using T = High_precision_floating_point_number<accuracy>;
        return exp(x) - T(1);
    }
    template<int accuracy>
    High_precision_floating_point_number<accuracy> log2(const High_precision_floating_point_number<accuracy>& x){
        return log(x) / _hp_ln2<accuracy>();
    }
    template<int accuracy>
    High_precision_floating_point_number<accuracy> log10(const High_precision_floating_point_number<accuracy>& x){
        return log(x) / _hp_ln10<accuracy>();
    }
    template<int accuracy>
    High_precision_floating_point_number<accuracy> log1p(const High_precision_floating_point_number<accuracy>& x){
        using T = High_precision_floating_point_number<accuracy>;
        return log(x + T(1));
    }
    template<int accuracy>
    High_precision_floating_point_number<accuracy> sin(const High_precision_floating_point_number<accuracy>& x){
        using T = High_precision_floating_point_number<accuracy>;
        T two_pi = _hp_pi<accuracy>() * T(2);
        T k = round(x / two_pi);
        long long ki = std::llround(k.to_double());
        T r = (x - T(ki) * two_pi).to_decimals(accuracy); //归约到 [-pi,pi]
        return _hp_sin_series(r);
    }
    template<int accuracy>
    High_precision_floating_point_number<accuracy> cos(const High_precision_floating_point_number<accuracy>& x){
        using T = High_precision_floating_point_number<accuracy>;
        T two_pi = _hp_pi<accuracy>() * T(2);
        T k = round(x / two_pi);
        long long ki = std::llround(k.to_double());
        T r = (x - T(ki) * two_pi).to_decimals(accuracy);
        return _hp_cos_series(r);
    }
    template<int accuracy>
    High_precision_floating_point_number<accuracy> tan(const High_precision_floating_point_number<accuracy>& x){
        return sin(x) / cos(x);
    }
    template<int accuracy>
    High_precision_floating_point_number<accuracy> atan(const High_precision_floating_point_number<accuracy>& x){
        using T = High_precision_floating_point_number<accuracy>;
        if(x < T(0)) return T(0) - atan(T(0) - x);
        if(x > T(1)) return _hp_pi<accuracy>() / T(2) - atan(T(1) / x);
        if(x > T(0.5)){ //接近 1 时直接级数慢，用 pi/4 半角变换 atan(x)=pi/4-atan((1-x)/(1+x))
            T t = (T(1) - x) / (T(1) + x);
            return _hp_pi<accuracy>() / T(4) - atan(t);
        }
        return _hp_atan_series(x);
    }
    template<int accuracy>
    High_precision_floating_point_number<accuracy> atan2(const High_precision_floating_point_number<accuracy>& y,
                                                          const High_precision_floating_point_number<accuracy>& x){
        using T = High_precision_floating_point_number<accuracy>;
        T pi = _hp_pi<accuracy>();
        if(x > T(0)) return atan(y / x);
        if(x < T(0)){
            if(y >= T(0)) return atan(y / x) + pi;
            else return atan(y / x) - pi;
        }
        if(y > T(0)) return pi / T(2);
        if(y < T(0)) return T(0) - pi / T(2);
        return T(0);
    }
    template<int accuracy>
    High_precision_floating_point_number<accuracy> asin(const High_precision_floating_point_number<accuracy>& x){
        using T = High_precision_floating_point_number<accuracy>;
        T root = sqrt(T(1) - x * x);
        return atan2(x, root);
    }
    template<int accuracy>
    High_precision_floating_point_number<accuracy> acos(const High_precision_floating_point_number<accuracy>& x){
        using T = High_precision_floating_point_number<accuracy>;
        return _hp_pi<accuracy>() / T(2) - asin(x);
    }
    template<int accuracy>
    High_precision_floating_point_number<accuracy> sinh(const High_precision_floating_point_number<accuracy>& x){
        using T = High_precision_floating_point_number<accuracy>;
        T e1 = exp(x), e2 = exp(T(0) - x);
        return (e1 - e2) / T(2);
    }
    template<int accuracy>
    High_precision_floating_point_number<accuracy> cosh(const High_precision_floating_point_number<accuracy>& x){
        using T = High_precision_floating_point_number<accuracy>;
        T e1 = exp(x), e2 = exp(T(0) - x);
        return (e1 + e2) / T(2);
    }
    template<int accuracy>
    High_precision_floating_point_number<accuracy> tanh(const High_precision_floating_point_number<accuracy>& x){
        return sinh(x) / cosh(x);
    }
    template<int accuracy>
    High_precision_floating_point_number<accuracy> hypot(const High_precision_floating_point_number<accuracy>& a,
                                                         const High_precision_floating_point_number<accuracy>& b){
        return sqrt(a * a + b * b);
    }

    //===== <algorithm> 值型函数（与 goodint 平行）=====
    template<int accuracy>
    High_precision_floating_point_number<accuracy> min(const High_precision_floating_point_number<accuracy>& a,
                                                        const High_precision_floating_point_number<accuracy>& b){
        return a < b ? a : b;
    }
    template<int accuracy>
    High_precision_floating_point_number<accuracy> max(const High_precision_floating_point_number<accuracy>& a,
                                                        const High_precision_floating_point_number<accuracy>& b){
        return a < b ? b : a;
    }
    template<int accuracy>
    std::pair<High_precision_floating_point_number<accuracy>,High_precision_floating_point_number<accuracy>>
    minmax(const High_precision_floating_point_number<accuracy>& a,
           const High_precision_floating_point_number<accuracy>& b){
        if(a < b) return {a,b};
        return {b,a};
    }
    template<int accuracy>
    High_precision_floating_point_number<accuracy> clamp(const High_precision_floating_point_number<accuracy>& v,
                                                         const High_precision_floating_point_number<accuracy>& lo,
                                                         const High_precision_floating_point_number<accuracy>& hi){
        return min(max(v, lo), hi);
    }

    //===== 流输入（cin >> gd），与 << 对称 =====
    template<int accuracy>
    std::istream& operator>>(std::istream& is, High_precision_floating_point_number<accuracy>& x){
        std::string s;
        if(!(is>>s)) return is;
        x=High_precision_floating_point_number<accuracy>(s);
        return is;
    }

    //===== 内置数值（左侧）与 gooddouble（右侧）的混合运算 =====
    //成员运算符只覆盖「gooddouble 在左」；内置在左时靠这些非成员模板经 ADL 解析。
    template<int accuracy, typename Num>
    typename std::enable_if<(std::is_integral<Num>::value && std::is_signed<Num>::value)
                            || std::is_floating_point<Num>::value,
                            High_precision_floating_point_number<accuracy>>::type
    operator+(Num a, const High_precision_floating_point_number<accuracy>& b){ return High_precision_floating_point_number<accuracy>(a)+b; }
    template<int accuracy, typename Num>
    typename std::enable_if<(std::is_integral<Num>::value && std::is_signed<Num>::value)
                            || std::is_floating_point<Num>::value,
                            High_precision_floating_point_number<accuracy>>::type
    operator-(Num a, const High_precision_floating_point_number<accuracy>& b){ return High_precision_floating_point_number<accuracy>(a)-b; }
    template<int accuracy, typename Num>
    typename std::enable_if<(std::is_integral<Num>::value && std::is_signed<Num>::value)
                            || std::is_floating_point<Num>::value,
                            High_precision_floating_point_number<accuracy>>::type
    operator*(Num a, const High_precision_floating_point_number<accuracy>& b){ return High_precision_floating_point_number<accuracy>(a)*b; }
    template<int accuracy, typename Num>
    typename std::enable_if<(std::is_integral<Num>::value && std::is_signed<Num>::value)
                            || std::is_floating_point<Num>::value,
                            High_precision_floating_point_number<accuracy>>::type
    operator/(Num a, const High_precision_floating_point_number<accuracy>& b){ return High_precision_floating_point_number<accuracy>(a)/b; }
    template<int accuracy, typename Num>
    typename std::enable_if<(std::is_integral<Num>::value && std::is_signed<Num>::value)
                            || std::is_floating_point<Num>::value,bool>::type
    operator<(Num a, const High_precision_floating_point_number<accuracy>& b){ return High_precision_floating_point_number<accuracy>(a)<b; }
    template<int accuracy, typename Num>
    typename std::enable_if<(std::is_integral<Num>::value && std::is_signed<Num>::value)
                            || std::is_floating_point<Num>::value,bool>::type
    operator>(Num a, const High_precision_floating_point_number<accuracy>& b){ return High_precision_floating_point_number<accuracy>(a)>b; }
    template<int accuracy, typename Num>
    typename std::enable_if<(std::is_integral<Num>::value && std::is_signed<Num>::value)
                            || std::is_floating_point<Num>::value,bool>::type
    operator<=(Num a, const High_precision_floating_point_number<accuracy>& b){ return High_precision_floating_point_number<accuracy>(a)<=b; }
    template<int accuracy, typename Num>
    typename std::enable_if<(std::is_integral<Num>::value && std::is_signed<Num>::value)
                            || std::is_floating_point<Num>::value,bool>::type
    operator>=(Num a, const High_precision_floating_point_number<accuracy>& b){ return High_precision_floating_point_number<accuracy>(a)>=b; }
    template<int accuracy, typename Num>
    typename std::enable_if<(std::is_integral<Num>::value && std::is_signed<Num>::value)
                            || std::is_floating_point<Num>::value,bool>::type
    operator==(Num a, const High_precision_floating_point_number<accuracy>& b){ return High_precision_floating_point_number<accuracy>(a)==b; }
    template<int accuracy, typename Num>
    typename std::enable_if<(std::is_integral<Num>::value && std::is_signed<Num>::value)
                            || std::is_floating_point<Num>::value,bool>::type
    operator!=(Num a, const High_precision_floating_point_number<accuracy>& b){ return High_precision_floating_point_number<accuracy>(a)!=b; }

    //===== goodint（左）与 gooddouble（右）的混合运算（gooddouble 在左时成员运算符 + 隐式提升即可）=====
    //A、B 独立模板参数：允许不同 accuracy 的大整数与高精度浮点混合，结果取较大精度
    template<int A, int B>
    High_precision_floating_point_number<(A > B ? A : B)> operator+(const High_precision_number<A>& a,
                                                                    const High_precision_floating_point_number<B>& b){
        using R = High_precision_floating_point_number<(A > B ? A : B)>;
        return R(a) + R(b);
    }
    template<int A, int B>
    High_precision_floating_point_number<(A > B ? A : B)> operator-(const High_precision_number<A>& a,
                                                                    const High_precision_floating_point_number<B>& b){
        using R = High_precision_floating_point_number<(A > B ? A : B)>;
        return R(a) - R(b);
    }
    template<int A, int B>
    High_precision_floating_point_number<(A > B ? A : B)> operator*(const High_precision_number<A>& a,
                                                                    const High_precision_floating_point_number<B>& b){
        using R = High_precision_floating_point_number<(A > B ? A : B)>;
        return R(a) * R(b);
    }
    template<int A, int B>
    High_precision_floating_point_number<(A > B ? A : B)> operator/(const High_precision_number<A>& a,
                                                                    const High_precision_floating_point_number<B>& b){
        using R = High_precision_floating_point_number<(A > B ? A : B)>;
        return R(a) / R(b);
    }
    template<int A, int B>
    bool operator<(const High_precision_number<A>& a, const High_precision_floating_point_number<B>& b){
        using R = High_precision_floating_point_number<(A > B ? A : B)>;
        return R(a) < R(b);
    }
    template<int A, int B>
    bool operator>(const High_precision_number<A>& a, const High_precision_floating_point_number<B>& b){
        using R = High_precision_floating_point_number<(A > B ? A : B)>;
        return R(a) > R(b);
    }
    template<int A, int B>
    bool operator<=(const High_precision_number<A>& a, const High_precision_floating_point_number<B>& b){
        using R = High_precision_floating_point_number<(A > B ? A : B)>;
        return R(a) <= R(b);
    }
    template<int A, int B>
    bool operator>=(const High_precision_number<A>& a, const High_precision_floating_point_number<B>& b){
        using R = High_precision_floating_point_number<(A > B ? A : B)>;
        return R(a) >= R(b);
    }
    template<int A, int B>
    bool operator==(const High_precision_number<A>& a, const High_precision_floating_point_number<B>& b){
        using R = High_precision_floating_point_number<(A > B ? A : B)>;
        return R(a) == R(b);
    }
    template<int A, int B>
    bool operator!=(const High_precision_number<A>& a, const High_precision_floating_point_number<B>& b){
        using R = High_precision_floating_point_number<(A > B ? A : B)>;
        return R(a) != R(b);
    }

    //===== <cmath> 函数接受 goodint 参数（薄封装：先提升为 gooddouble 再计算）=====
    //与 goodint 自身已有的 abs/min/max/pow(hpn,ll)/powmod 等不冲突；
    //gooddouble 在左、goodint 在右的混合（如 pow(gd, hpn)）经隐式提升自动匹配 gooddouble 版。
    template<int accuracy>
    High_precision_floating_point_number<accuracy> sqrt(const High_precision_number<accuracy>& x){
        return sqrt(High_precision_floating_point_number<accuracy>(x));
    }
    template<int accuracy>
    High_precision_floating_point_number<accuracy> cbrt(const High_precision_number<accuracy>& x){
        return cbrt(High_precision_floating_point_number<accuracy>(x));
    }
    template<int accuracy>
    High_precision_floating_point_number<accuracy> exp(const High_precision_number<accuracy>& x){
        return exp(High_precision_floating_point_number<accuracy>(x));
    }
    template<int accuracy>
    High_precision_floating_point_number<accuracy> log(const High_precision_number<accuracy>& x){
        return log(High_precision_floating_point_number<accuracy>(x));
    }
    template<int accuracy>
    High_precision_floating_point_number<accuracy> log2(const High_precision_number<accuracy>& x){
        return log2(High_precision_floating_point_number<accuracy>(x));
    }
    template<int accuracy>
    High_precision_floating_point_number<accuracy> log10(const High_precision_number<accuracy>& x){
        return log10(High_precision_floating_point_number<accuracy>(x));
    }
    template<int accuracy>
    High_precision_floating_point_number<accuracy> log1p(const High_precision_number<accuracy>& x){
        return log1p(High_precision_floating_point_number<accuracy>(x));
    }
    template<int accuracy>
    High_precision_floating_point_number<accuracy> exp2(const High_precision_number<accuracy>& x){
        return exp2(High_precision_floating_point_number<accuracy>(x));
    }
    template<int accuracy>
    High_precision_floating_point_number<accuracy> expm1(const High_precision_number<accuracy>& x){
        return expm1(High_precision_floating_point_number<accuracy>(x));
    }
    template<int accuracy>
    High_precision_floating_point_number<accuracy> sin(const High_precision_number<accuracy>& x){
        return sin(High_precision_floating_point_number<accuracy>(x));
    }
    template<int accuracy>
    High_precision_floating_point_number<accuracy> cos(const High_precision_number<accuracy>& x){
        return cos(High_precision_floating_point_number<accuracy>(x));
    }
    template<int accuracy>
    High_precision_floating_point_number<accuracy> tan(const High_precision_number<accuracy>& x){
        return tan(High_precision_floating_point_number<accuracy>(x));
    }
    template<int accuracy>
    High_precision_floating_point_number<accuracy> asin(const High_precision_number<accuracy>& x){
        return asin(High_precision_floating_point_number<accuracy>(x));
    }
    template<int accuracy>
    High_precision_floating_point_number<accuracy> acos(const High_precision_number<accuracy>& x){
        return acos(High_precision_floating_point_number<accuracy>(x));
    }
    template<int accuracy>
    High_precision_floating_point_number<accuracy> atan(const High_precision_number<accuracy>& x){
        return atan(High_precision_floating_point_number<accuracy>(x));
    }
    template<int accuracy>
    High_precision_floating_point_number<accuracy> atan2(const High_precision_number<accuracy>& y, const High_precision_number<accuracy>& x){
        return atan2(High_precision_floating_point_number<accuracy>(y), High_precision_floating_point_number<accuracy>(x));
    }
    template<int accuracy>
    High_precision_floating_point_number<accuracy> sinh(const High_precision_number<accuracy>& x){
        return sinh(High_precision_floating_point_number<accuracy>(x));
    }
    template<int accuracy>
    High_precision_floating_point_number<accuracy> cosh(const High_precision_number<accuracy>& x){
        return cosh(High_precision_floating_point_number<accuracy>(x));
    }
    template<int accuracy>
    High_precision_floating_point_number<accuracy> tanh(const High_precision_number<accuracy>& x){
        return tanh(High_precision_floating_point_number<accuracy>(x));
    }
    template<int accuracy>
    High_precision_floating_point_number<accuracy> hypot(const High_precision_number<accuracy>& a, const High_precision_number<accuracy>& b){
        return hypot(High_precision_floating_point_number<accuracy>(a), High_precision_floating_point_number<accuracy>(b));
    }
    template<int accuracy>
    High_precision_floating_point_number<accuracy> pow(const High_precision_number<accuracy>& a, const High_precision_number<accuracy>& b){
        return pow(High_precision_floating_point_number<accuracy>(a), High_precision_floating_point_number<accuracy>(b));
    }
    template<int accuracy>
    High_precision_floating_point_number<accuracy> fmod(const High_precision_number<accuracy>& a, const High_precision_number<accuracy>& b){
        return fmod(High_precision_floating_point_number<accuracy>(a), High_precision_floating_point_number<accuracy>(b));
    }
    template<int accuracy>
    High_precision_floating_point_number<accuracy> remainder(const High_precision_number<accuracy>& a, const High_precision_number<accuracy>& b){
        return remainder(High_precision_floating_point_number<accuracy>(a), High_precision_floating_point_number<accuracy>(b));
    }
    template<int accuracy>
    High_precision_floating_point_number<accuracy> trunc(const High_precision_number<accuracy>& x){
        return trunc(High_precision_floating_point_number<accuracy>(x));
    }
    template<int accuracy>
    High_precision_floating_point_number<accuracy> floor(const High_precision_number<accuracy>& x){
        return floor(High_precision_floating_point_number<accuracy>(x));
    }
    template<int accuracy>
    High_precision_floating_point_number<accuracy> ceil(const High_precision_number<accuracy>& x){
        return ceil(High_precision_floating_point_number<accuracy>(x));
    }
    template<int accuracy>
    High_precision_floating_point_number<accuracy> round(const High_precision_number<accuracy>& x){
        return round(High_precision_floating_point_number<accuracy>(x));
    }
    template<int accuracy>
    High_precision_floating_point_number<accuracy> fma(const High_precision_number<accuracy>& a,
                                                       const High_precision_number<accuracy>& b,
                                                       const High_precision_number<accuracy>& c){
        return fma(High_precision_floating_point_number<accuracy>(a), High_precision_floating_point_number<accuracy>(b), High_precision_floating_point_number<accuracy>(c));
    }
    template<int accuracy>
    High_precision_floating_point_number<accuracy> copysign(const High_precision_number<accuracy>& a, const High_precision_number<accuracy>& b){
        return copysign(High_precision_floating_point_number<accuracy>(a), High_precision_floating_point_number<accuracy>(b));
    }
    template<int accuracy>
    High_precision_floating_point_number<accuracy> fdim(const High_precision_number<accuracy>& a, const High_precision_number<accuracy>& b){
        return fdim(High_precision_floating_point_number<accuracy>(a), High_precision_floating_point_number<accuracy>(b));
    }
    template<int accuracy>
    High_precision_floating_point_number<accuracy> fabs(const High_precision_number<accuracy>& x){
        return fabs(High_precision_floating_point_number<accuracy>(x));
    }
    template<int accuracy>
    High_precision_floating_point_number<accuracy> fmax(const High_precision_number<accuracy>& a, const High_precision_number<accuracy>& b){
        return fmax(High_precision_floating_point_number<accuracy>(a), High_precision_floating_point_number<accuracy>(b));
    }
    template<int accuracy>
    High_precision_floating_point_number<accuracy> fmin(const High_precision_number<accuracy>& a, const High_precision_number<accuracy>& b){
        return fmin(High_precision_floating_point_number<accuracy>(a), High_precision_floating_point_number<accuracy>(b));
    }
    template<int accuracy>
    bool signbit(const High_precision_number<accuracy>& x){
        return signbit(High_precision_floating_point_number<accuracy>(x));
    }
}

//===== 对 High_precision_floating_point_number 偏特化 std::numeric_limits（标准允许）=====
//任意精度浮点没有真正的最大/最小，min()/lowest()/max() 仅作编译占位返回 0；
//若用作 DP 的 INF 哨兵，请显式传入 INF 参数。
namespace std{
    template<int accuracy>
    struct numeric_limits<better_std::High_precision_floating_point_number<accuracy>>{
        static constexpr bool is_specialized = true;
        static constexpr bool is_signed = true;
        static constexpr bool is_integer = false;
        static constexpr bool is_exact = false;
        static constexpr bool has_infinity = false;
        static constexpr bool has_quiet_NaN = false;
        static constexpr bool has_signaling_NaN = false;
        static constexpr bool is_bounded = false;
        static constexpr bool is_modulo = false;
        static constexpr int digits = 0;
        static constexpr int digits10 = 0;
        static constexpr int max_digits10 = 0;
        static constexpr int radix = 10;
        static constexpr int min_exponent = 0;
        static constexpr int min_exponent10 = 0;
        static constexpr int max_exponent = 0;
        static constexpr int max_exponent10 = 0;
        static constexpr bool traps = false;
        static constexpr bool tinyness_before = false;
        static constexpr float_denorm_style has_denorm = denorm_absent;
        static constexpr bool has_denorm_loss = false;
        static better_std::High_precision_floating_point_number<accuracy> min() noexcept{ return better_std::High_precision_floating_point_number<accuracy>(0); }
        static better_std::High_precision_floating_point_number<accuracy> lowest() noexcept{ return better_std::High_precision_floating_point_number<accuracy>(0); }
        static better_std::High_precision_floating_point_number<accuracy> max() noexcept{ return better_std::High_precision_floating_point_number<accuracy>(0); }
        static better_std::High_precision_floating_point_number<accuracy> epsilon() noexcept{ return better_std::High_precision_floating_point_number<accuracy>(0); }
        static better_std::High_precision_floating_point_number<accuracy> round_error() noexcept{ return better_std::High_precision_floating_point_number<accuracy>(0); }
        static better_std::High_precision_floating_point_number<accuracy> infinity() noexcept{ return better_std::High_precision_floating_point_number<accuracy>(0); }
        static better_std::High_precision_floating_point_number<accuracy> quiet_NaN() noexcept{ return better_std::High_precision_floating_point_number<accuracy>(0); }
        static better_std::High_precision_floating_point_number<accuracy> signaling_NaN() noexcept{ return better_std::High_precision_floating_point_number<accuracy>(0); }
        static better_std::High_precision_floating_point_number<accuracy> denorm_min() noexcept{ return better_std::High_precision_floating_point_number<accuracy>(0); }
    };
}

