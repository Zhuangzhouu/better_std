#pragma once
#include<iostream>
#include<vector>
#include<string>
#include<algorithm>
#include<utility>
#include<functional>
#include<type_traits>
#include<limits>
namespace better_std{
    //高精度非负/有符号大整数，每个元素存一个十进制位(0~9)，小端序
    //accuracy 仅作为精度语义标签（十进制位数），实际位数随数据动态增长
    template<int> class High_precision_number;
    template<int A> std::ostream& operator<<(std::ostream& os,const High_precision_number<A>& x);
    template<int accuracy>
    class High_precision_number{
        private:
            std::vector<int> dig;//little-endian，dig[i] ∈ [0,9]
            bool neg;
            void normalize(){
                while(dig.size()>1 && dig.back()==0) dig.pop_back();
                if(dig.size()==1 && dig[0]==0) neg=false;
            }
        public:
            template<int> friend class High_precision_number;
            template<int A> friend std::ostream& operator<<(std::ostream& os,const High_precision_number<A>& x);

            High_precision_number():dig(1,0),neg(false){}
            High_precision_number(long long v){
                neg=(v<0);
                unsigned long long uv=neg?0ull-(unsigned long long)v:(unsigned long long)v;//LLONG_MIN 安全
                if(uv==0){dig={0};return;}
                dig.clear();
                while(uv>0){dig.push_back((int)(uv%10));uv/=10;}
            }
            High_precision_number(const std::string& s){
                neg=false;dig.clear();
                size_t i=0;
                if(s.size()>0 && (s[0]=='-'||s[0]=='+')){neg=(s[0]=='-');i=1;}
                bool any=false;
                for(size_t j=s.size();j>i;){
                    j--;
                    if(s[j]>='0'&&s[j]<='9'){dig.push_back(s[j]-'0');any=true;}
                }
                if(!any)dig={0};
                normalize();
            }
            template<int U>
            High_precision_number(const High_precision_number<U>& o):dig(o.dig),neg(o.neg){}

            int size()const{return (int)dig.size();}
            bool is_neg()const{return neg;}
            std::string to_string() const{
                std::string s;
                if(neg)s+='-';
                for(int i=(int)dig.size()-1;i>=0;i--) s+=char('0'+dig[i]);
                if(s.empty()||s=="-")s="0";
                return s;
            }
            double to_double() const{
                double v=0;
                for(int i=(int)dig.size()-1;i>=0;i--) v=v*10+dig[i];
                return neg?-v:v;
            }

            bool operator<(const High_precision_number& o) const{
                if(neg!=o.neg) return neg;
                if(dig.size()!=o.dig.size()) return neg?dig.size()>o.dig.size():dig.size()<o.dig.size();
                for(int i=(int)dig.size()-1;i>=0;i--) if(dig[i]!=o.dig[i]) return neg?dig[i]>o.dig[i]:dig[i]<o.dig[i];
                return false;
            }
            bool operator==(const High_precision_number& o) const{return neg==o.neg && dig==o.dig;}
            bool operator>(const High_precision_number& o) const{return o<*this;}
            bool operator<=(const High_precision_number& o) const{return !(*this>o);}
            bool operator>=(const High_precision_number& o) const{return !(*this<o);}
            bool operator!=(const High_precision_number& o) const{return !(*this==o);}

            //内部工具：对两个非负 magnitude 直接做加/减（sub 要求 a>=b）
            static High_precision_number add_abs(const High_precision_number& a,const High_precision_number& b){
                High_precision_number r;
                int carry=0;int n=std::max((int)a.dig.size(),(int)b.dig.size());
                r.dig.assign(n,0);
                for(int i=0;i<n;i++){
                    int av=(i<(int)a.dig.size())?a.dig[i]:0;
                    int bv=(i<(int)b.dig.size())?b.dig[i]:0;
                    int s=av+bv+carry;r.dig[i]=s%10;carry=s/10;
                }
                if(carry)r.dig.push_back(carry);
                r.normalize();return r;
            }
            static High_precision_number sub_abs(const High_precision_number& a,const High_precision_number& b){
                High_precision_number r;
                r.dig.assign(a.dig.size(),0);
                int borrow=0;
                for(int i=0;i<(int)a.dig.size();i++){
                    int av=a.dig[i];
                    int bv=(i<(int)b.dig.size())?b.dig[i]:0;
                    int s=av-bv-borrow;
                    if(s<0){s+=10;borrow=1;}else borrow=0;
                    r.dig[i]=s;
                }
                r.normalize();return r;
            }
            High_precision_number operator+() const{return *this;}
            High_precision_number operator-() const{High_precision_number r=*this;if(!(r.dig.size()==1&&r.dig[0]==0))r.neg=!r.neg;return r;}
            High_precision_number operator+(const High_precision_number& o) const{
                if(neg==o.neg){
                    High_precision_number r=add_abs(*this,o);
                    r.neg=neg;
                    return r;
                }
                High_precision_number a=*this;a.neg=false;
                High_precision_number b=o;b.neg=false;
                if(a<b){High_precision_number r=sub_abs(b,a);r.neg=o.neg;return r;}
                High_precision_number r=sub_abs(a,b);r.neg=neg;return r;
            }
            High_precision_number operator-(const High_precision_number& o) const{
                if(neg!=o.neg){
                    High_precision_number b=o;b.neg=!b.neg;
                    return *this+b;
                }
                High_precision_number a=*this;a.neg=false;
                High_precision_number b=o;b.neg=false;
                High_precision_number r;
                if(a<b){r=sub_abs(b,a);r.neg=!neg;}
                else{r=sub_abs(a,b);r.neg=neg;}
                return r;
            }
            High_precision_number operator*(const High_precision_number& o) const{
                High_precision_number r;
                r.dig.assign(dig.size()+o.dig.size(),0);
                bool res_neg=neg!=o.neg;
                for(size_t i=0;i<dig.size();i++){
                    long long carry=0;
                    for(size_t j=0;j<o.dig.size();j++){
                        long long cur=(long long)r.dig[i+j]+(long long)dig[i]*o.dig[j]+carry;
                        r.dig[i+j]=(int)(cur%10);carry=cur/10;
                    }
                    size_t k=i+o.dig.size();
                    while(carry){
                        if(k>=r.dig.size())r.dig.push_back(0);
                        long long cur=(long long)r.dig[k]+carry;
                        r.dig[k]=(int)(cur%10);carry=cur/10;k++;
                    }
                }
                r.normalize();
                if(r.dig.size()==1 && r.dig[0]==0)r.neg=false;else r.neg=res_neg;
                return r;
            }
            //整数除法与取模（向零取整，符号规则与 C++ 一致）
            std::pair<High_precision_number,High_precision_number> divmod(const High_precision_number& o) const{
                High_precision_number a=*this;a.neg=false;
                High_precision_number b=o;b.neg=false;
                bool res_neg=neg!=o.neg;
                if(b.dig.size()==1 && b.dig[0]==0) return {High_precision_number(0),High_precision_number(0)};
                std::vector<int> qd;
                High_precision_number rem;rem.dig={0};
                for(int i=(int)a.dig.size()-1;i>=0;i--){
                    rem=rem*High_precision_number(10);
                    rem=rem+High_precision_number(a.dig[i]);
                    int cnt=0;
                    while(!(rem<b)){rem=rem-b;cnt++;}
                    qd.push_back(cnt);
                }
                if(qd.empty())qd.push_back(0);
                std::reverse(qd.begin(),qd.end());
                High_precision_number q;q.dig=qd;
                q.normalize();
                if(!(q.dig.size()==1&&q.dig[0]==0))q.neg=res_neg;
                rem.normalize();
                //余数符号须与被除数一致（C++ 向零取整语义：a = q*b + r 且 r 与 a 同号）
                if(!(rem.dig.size()==1&&rem.dig[0]==0)) rem.neg=neg;
                return {q,rem};
            }
            High_precision_number operator/(const High_precision_number& o) const{return divmod(o).first;}
            High_precision_number operator%(const High_precision_number& o) const{return divmod(o).second;}

            //===== 复合赋值 / 自增自减（参数为内置整数时经非 explicit 构造隐式转换）=====
            High_precision_number& operator+=(const High_precision_number& o){*this=*this+o;return *this;}
            High_precision_number& operator-=(const High_precision_number& o){*this=*this-o;return *this;}
            High_precision_number& operator*=(const High_precision_number& o){*this=*this*o;return *this;}
            High_precision_number& operator/=(const High_precision_number& o){*this=*this/o;return *this;}
            High_precision_number& operator%=(const High_precision_number& o){*this=*this%o;return *this;}
            High_precision_number& operator++(){*this=*this+High_precision_number(1);return *this;}
            High_precision_number operator++(int){High_precision_number t=*this;++(*this);return t;}
            High_precision_number& operator--(){*this=*this-High_precision_number(1);return *this;}
            High_precision_number operator--(int){High_precision_number t=*this;--(*this);return t;}

            //===== 便捷查询 =====
            bool is_zero()const{return dig.size()==1&&dig[0]==0;}
            bool is_odd()const{return !is_zero()&&(dig[0]&1)==1;}
            //值落在 long long 范围内时精确转换；溢出则按 2^64 补码截断（竞赛可用）
            long long to_long_long()const{
                unsigned long long v=0;
                for(int i=(int)dig.size()-1;i>=0;i--) v=v*10ull+(unsigned long long)dig[i];
                if(neg) return (long long)(0ull-v);
                return (long long)v;
            }
    };
    template<int A>
    std::ostream& operator<<(std::ostream& os,const High_precision_number<A>& x){
        if(x.neg)os<<'-';
        for(int i=(int)x.dig.size()-1;i>=0;i--) os<<x.dig[i];
        return os;
    }

    //===== 标准库「int 输入」函数的重载 =====
    //这些函数原本只在 namespace std 中以 int/long long 等内建类型提供，
    //High_precision_number 无法隐式匹配，故在 better_std 中提供重载，
    //借由 ADL（参数类型所在命名空间）即可被 unqualified 调用解析到。
    //注意：不能向 namespace std 添加重载（属未定义行为），因此诸如
    //std::abs(hpn) / std::min(hpn,hpn) 这种写法不会被解析，
    //请用 abs(hpn) / min(a,b) 或 better_std::min(a,b)。
    //
    //（一）非 <algorithm> 部分：abs / div / to_string / gcd / lcm（gcd/lcm 属 <numeric>）
    //（二）<algorithm> 的「值类型 int 输入」函数：min / max / minmax / clamp
    //      它们是 std::min(int,int) 等族的对应物，对大整数做值选择/夹取；
    //      其余 <algorithm> 函数（sort/find/count/equal/binary_search/lower_bound…）
    //      为区间模板，本就能借已有的 operator< / operator== 接受
    //      High_precision_number，无需也无法做有意为之的重载；而 fill_n/advance
    //      等以 int 作「计数/步长」而非「值」，亦不涉及大整数参数。

    //与 std::div_t 对应：保存商与余数
    template<int accuracy>
    struct div_t{ High_precision_number<accuracy> quot; High_precision_number<accuracy> rem; };

    template<int accuracy>
    High_precision_number<accuracy> abs(const High_precision_number<accuracy>& x){
        High_precision_number<accuracy> z(0);
        if(x<z) return z-x;   //负 → 取相反数（0 - x）
        return x;
    }
    template<int accuracy>
    div_t<accuracy> div(const High_precision_number<accuracy>& a,const High_precision_number<accuracy>& b){
        auto p=a.divmod(b);
        return {p.first,p.second};
    }
    template<int accuracy>
    std::string to_string(const High_precision_number<accuracy>& x){ return x.to_string(); }

    //std::gcd/std::lcm 是模板且内部 static_assert(is_integral_v)，大整数无法匹配，
    //故在 better_std 中按欧几里得算法提供（属 <numeric>，不在“算法库”排除范围内）。
    template<int accuracy>
    High_precision_number<accuracy> gcd(High_precision_number<accuracy> a,High_precision_number<accuracy> b){
        a=abs(a);b=abs(b);
        High_precision_number<accuracy> z(0);
        while(!(b==z)){ High_precision_number<accuracy> t=b; b=a%b; a=t; }
        return a;
    }
    template<int accuracy>
    High_precision_number<accuracy> lcm(const High_precision_number<accuracy>& a,const High_precision_number<accuracy>& b){
        High_precision_number<accuracy> z(0);
        if(a==z||b==z) return z;
        return abs(a)/gcd(a,b)*abs(b);
    }

    //===== <algorithm> 的「值类型 int 输入」函数重载 =====
    //对应 std::min / std::max / std::minmax / std::clamp（clamp 为 C++17）。
    //它们对内建 int 就是这批函数的典型用例，这里为 High_precision_number 提供，
    //使 min(a,b) / clamp(x,lo,hi) 等写法经 ADL 解析到本命名空间。
    template<int accuracy>
    High_precision_number<accuracy> min(const High_precision_number<accuracy>& a,const High_precision_number<accuracy>& b){
        return a<b?a:b;
    }
    template<int accuracy>
    High_precision_number<accuracy> max(const High_precision_number<accuracy>& a,const High_precision_number<accuracy>& b){
        return a<b?b:a;
    }
    template<int accuracy>
    std::pair<High_precision_number<accuracy>,High_precision_number<accuracy>>
    minmax(const High_precision_number<accuracy>& a,const High_precision_number<accuracy>& b){
        if(a<b) return {a,b};
        return {b,a};
    }
    template<int accuracy>
    High_precision_number<accuracy> clamp(const High_precision_number<accuracy>& v,
                                          const High_precision_number<accuracy>& lo,
                                          const High_precision_number<accuracy>& hi){
        return min(max(v,lo),hi);
    }

    //===== 内置整数（左侧）与大整数（右侧）的混合运算 =====
    //成员运算符只能处理「大整数在左」；内置类型在左时靠这些非成员模板经 ADL 解析。
    //仅对有符号整型生效（无符号大值会截断进 long long 构造，语义模糊故不支持）。
    template<int accuracy, typename Int>
    typename std::enable_if<std::is_integral<Int>::value && std::is_signed<Int>::value,
                            High_precision_number<accuracy>>::type
    operator+(Int a, const High_precision_number<accuracy>& b){ return High_precision_number<accuracy>(a)+b; }
    template<int accuracy, typename Int>
    typename std::enable_if<std::is_integral<Int>::value && std::is_signed<Int>::value,
                            High_precision_number<accuracy>>::type
    operator-(Int a, const High_precision_number<accuracy>& b){ return High_precision_number<accuracy>(a)-b; }
    template<int accuracy, typename Int>
    typename std::enable_if<std::is_integral<Int>::value && std::is_signed<Int>::value,
                            High_precision_number<accuracy>>::type
    operator*(Int a, const High_precision_number<accuracy>& b){ return High_precision_number<accuracy>(a)*b; }
    template<int accuracy, typename Int>
    typename std::enable_if<std::is_integral<Int>::value && std::is_signed<Int>::value,
                            High_precision_number<accuracy>>::type
    operator/(Int a, const High_precision_number<accuracy>& b){ return High_precision_number<accuracy>(a)/b; }
    template<int accuracy, typename Int>
    typename std::enable_if<std::is_integral<Int>::value && std::is_signed<Int>::value,
                            High_precision_number<accuracy>>::type
    operator%(Int a, const High_precision_number<accuracy>& b){ return High_precision_number<accuracy>(a)%b; }
    template<int accuracy, typename Int>
    typename std::enable_if<std::is_integral<Int>::value && std::is_signed<Int>::value,bool>::type
    operator<(Int a, const High_precision_number<accuracy>& b){ return High_precision_number<accuracy>(a)<b; }
    template<int accuracy, typename Int>
    typename std::enable_if<std::is_integral<Int>::value && std::is_signed<Int>::value,bool>::type
    operator>(Int a, const High_precision_number<accuracy>& b){ return High_precision_number<accuracy>(a)>b; }
    template<int accuracy, typename Int>
    typename std::enable_if<std::is_integral<Int>::value && std::is_signed<Int>::value,bool>::type
    operator<=(Int a, const High_precision_number<accuracy>& b){ return High_precision_number<accuracy>(a)<=b; }
    template<int accuracy, typename Int>
    typename std::enable_if<std::is_integral<Int>::value && std::is_signed<Int>::value,bool>::type
    operator>=(Int a, const High_precision_number<accuracy>& b){ return High_precision_number<accuracy>(a)>=b; }
    template<int accuracy, typename Int>
    typename std::enable_if<std::is_integral<Int>::value && std::is_signed<Int>::value,bool>::type
    operator==(Int a, const High_precision_number<accuracy>& b){ return High_precision_number<accuracy>(a)==b; }
    template<int accuracy, typename Int>
    typename std::enable_if<std::is_integral<Int>::value && std::is_signed<Int>::value,bool>::type
    operator!=(Int a, const High_precision_number<accuracy>& b){ return High_precision_number<accuracy>(a)!=b; }

    //===== 流输入（cin >> hpn），与 << 对称 =====
    template<int accuracy>
    std::istream& operator>>(std::istream& is, High_precision_number<accuracy>& x){
        std::string s;
        if(!(is>>s)) return is;
        x=High_precision_number<accuracy>(s);
        return is;
    }

    //===== 大整数常用数论函数 =====
    //整数幂（指数为内置整数且非负；底数可为负；exp<0 返回 0）
    template<int accuracy>
    High_precision_number<accuracy> pow(const High_precision_number<accuracy>& base, long long exp){
        if(exp<0) return High_precision_number<accuracy>(0);
        High_precision_number<accuracy> r(1), b=base;
        unsigned long long e=(unsigned long long)exp;
        while(e>0){ if(e&1ull) r=r*b; b=b*b; e>>=1; }
        return r;
    }
    //模幂 base^exp mod mod（exp、mod 可为大整数；负底数按数学模处理）
    template<int accuracy>
    High_precision_number<accuracy> powmod(const High_precision_number<accuracy>& base,
                                           const High_precision_number<accuracy>& exp,
                                           const High_precision_number<accuracy>& mod){
        High_precision_number<accuracy> z(0), one(1), two(2);
        if(mod==z||mod==one) return z;
        High_precision_number<accuracy> b=base%mod;
        if(b<z) b=b+mod;
        High_precision_number<accuracy> r(1), e=exp;
        while(!(e==z)){
            if(e.is_odd()) r=(r*b)%mod;
            b=(b*b)%mod;
            e=e/two;
        }
        return r;
    }
    //整数平方根（向下取整；负数返回 0）
    template<int accuracy>
    High_precision_number<accuracy> isqrt(const High_precision_number<accuracy>& x){
        High_precision_number<accuracy> z(0);
        if(x<z) return z;
        High_precision_number<accuracy> lo(0), hi=x, ans(0);
        while(lo<=hi){
            High_precision_number<accuracy> mid=(lo+hi)/High_precision_number<accuracy>(2);
            High_precision_number<accuracy> sq=mid*mid;
            if(sq<=x){ ans=mid; lo=mid+High_precision_number<accuracy>(1); }
            else hi=mid-High_precision_number<accuracy>(1);
        }
        return ans;
    }
    //阶乘 n!（n>=0，结果为大整数）
    template<int accuracy>
    High_precision_number<accuracy> factorial(long long n){
        High_precision_number<accuracy> r(1);
        for(long long i=2;i<=n;i++) r=r*High_precision_number<accuracy>(i);
        return r;
    }
    //组合数 C(n,k)（k 超界返回 0；n<0 返回 0）
    template<int accuracy>
    High_precision_number<accuracy> choose(long long n, long long k){
        if(n<0||k<0||k>n) return High_precision_number<accuracy>(0);
        if(k>n-k) k=n-k;
        High_precision_number<accuracy> r(1);
        for(long long i=0;i<k;i++) r=r*High_precision_number<accuracy>(n-i)/High_precision_number<accuracy>(i+1);
        return r;
    }
}

//===== 对 High_precision_number 偏特化 std::hash（标准允许）=====
//使得大整数可作为 unordered_map / unordered_set 的键
namespace std{
    template<int accuracy>
    struct hash<better_std::High_precision_number<accuracy>>{
        size_t operator()(const better_std::High_precision_number<accuracy>& x) const noexcept{
            return hash<string>{}(x.to_string());
        }
    };

    //===== 对 High_precision_number 偏特化 std::numeric_limits（标准允许）=====
    //任意精度类型没有真正的最大/最小，min()/lowest()/max() 仅作编译占位返回 0；
    //若用作 DP 的 INF 哨兵（如 sliding_window_min_dp），请显式传入 INF 参数。
    template<int accuracy>
    struct numeric_limits<better_std::High_precision_number<accuracy>>{
        static constexpr bool is_specialized = true;
        static constexpr bool is_signed = true;
        static constexpr bool is_integer = true;
        static constexpr bool is_exact = true;
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
        static better_std::High_precision_number<accuracy> min() noexcept{ return better_std::High_precision_number<accuracy>(0); }
        static better_std::High_precision_number<accuracy> lowest() noexcept{ return better_std::High_precision_number<accuracy>(0); }
        static better_std::High_precision_number<accuracy> max() noexcept{ return better_std::High_precision_number<accuracy>(0); }
        static better_std::High_precision_number<accuracy> epsilon() noexcept{ return better_std::High_precision_number<accuracy>(0); }
        static better_std::High_precision_number<accuracy> round_error() noexcept{ return better_std::High_precision_number<accuracy>(0); }
        static better_std::High_precision_number<accuracy> infinity() noexcept{ return better_std::High_precision_number<accuracy>(0); }
        static better_std::High_precision_number<accuracy> quiet_NaN() noexcept{ return better_std::High_precision_number<accuracy>(0); }
        static better_std::High_precision_number<accuracy> signaling_NaN() noexcept{ return better_std::High_precision_number<accuracy>(0); }
        static better_std::High_precision_number<accuracy> denorm_min() noexcept{ return better_std::High_precision_number<accuracy>(0); }
    };
}
