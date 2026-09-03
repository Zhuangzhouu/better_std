#pragma once
#include<iostream>
#include<vector>
#include<utility>
#include<algorithm>
#include<complex>
#include<cmath>
namespace better_std{
    //多项式：系数 a[i] 是 x^i 的系数，次数 = size-1，高次零自动裁剪
    //T 需支持 + - * /（长除用；double/fraction/高精度浮点皆可，int 除不尽请用分数）
    template<typename T=double>
    class std_polynomial{
        private:
            std::vector<T> a_;
            void trim(){
                while(a_.size()>1 && a_.back()==T()) a_.pop_back();
                if(a_.empty()) a_.push_back(T());
            }
        public:
            std_polynomial():a_(1,T()){}
            std_polynomial(const T& c):a_(1,c){}
            std_polynomial(const std::vector<T>& a):a_(a){ trim(); }
            int degree() const{ return (int)a_.size()-1; }
            int size() const{ return (int)a_.size(); }
            const T& operator[](int i) const{ return a_[i]; }
            T& operator[](int i){ return a_[i]; }
            bool operator==(const std_polynomial& o) const{ return a_==o.a_; }
            bool operator!=(const std_polynomial& o) const{ return !(*this==o); }

            std_polynomial operator+(const std_polynomial& o) const{
                std::vector<T> r(std::max(a_.size(),o.a_.size()),T());
                for(size_t i=0;i<a_.size();i++) r[i]+=a_[i];
                for(size_t i=0;i<o.a_.size();i++) r[i]+=o.a_[i];
                return std_polynomial(r);
            }
            std_polynomial operator-(const std_polynomial& o) const{
                std::vector<T> r(std::max(a_.size(),o.a_.size()),T());
                for(size_t i=0;i<a_.size();i++) r[i]+=a_[i];
                for(size_t i=0;i<o.a_.size();i++) r[i]-=o.a_[i];
                return std_polynomial(r);
            }
            std_polynomial operator-() const{
                std::vector<T> r(a_.size());
                for(size_t i=0;i<a_.size();i++) r[i]=T(0)-a_[i];
                return std_polynomial(r);
            }
            //卷积 O(n²)
            std_polynomial operator*(const std_polynomial& o) const{
                std::vector<T> r(a_.size()+o.a_.size()-1,T());
                for(size_t i=0;i<a_.size();i++)
                    for(size_t j=0;j<o.a_.size();j++)
                        r[i+j]+=a_[i]*o.a_[j];
                return std_polynomial(r);
            }
            std_polynomial operator*(const T& s) const{
                std::vector<T> r(a_.size());
                for(size_t i=0;i<a_.size();i++) r[i]=a_[i]*s;
                return std_polynomial(r);
            }
            //乘以 x^k
            std_polynomial shift(int k) const{
                if(k<=0) return *this;
                std::vector<T> r((size_t)k+a_.size(),T());
                for(size_t i=0;i<a_.size();i++) r[i+(size_t)k]=a_[i];
                return std_polynomial(r);
            }
            //霍纳求值
            T evaluate(const T& x) const{
                T r=T();
                for(int i=(int)a_.size()-1;i>=0;i--) r=r*x+a_[i];
                return r;
            }
            std_polynomial derivative() const{
                if(a_.size()<=1) return std_polynomial(T());
                std::vector<T> r(a_.size()-1);
                for(size_t i=1;i<a_.size();i++) r[i-1]=a_[i]*T((int)i);
                return std_polynomial(r);
            }
            std_polynomial integral() const{
                std::vector<T> r(a_.size()+1,T());
                for(size_t i=0;i<a_.size();i++) r[i+1]=a_[i]/T((int)i+1);
                return std_polynomial(r);
            }
            //多项式长除：返回 {商, 余}
            std::pair<std_polynomial,std_polynomial> divmod(const std_polynomial& o) const{
                std::vector<T> q(a_.size(),T());
                std::vector<T> r=a_;
                int m=o.degree();
                for(int i=degree();i>=m;i--){
                    T c=r[i]/o.a_[m];
                    q[i-m]=c;
                    for(int j=0;j<=m;j++) r[i-m+j]-=c*o.a_[j];
                }
                std_polynomial Q(q),R(r);
                Q.trim();R.trim();
                return {Q,R};
            }
            std_polynomial operator/(const std_polynomial& o) const{ return divmod(o).first; }
            std_polynomial operator%(const std_polynomial& o) const{ return divmod(o).second; }
            void print() const{
                bool first=true;
                for(int i=degree();i>=0;i--){
                    if(a_[i]==T()) continue;
                    if(!first){ if(a_[i]>T(0)) std::cout<<"+"; }
                    std::cout<<a_[i];
                    if(i>0){ std::cout<<"x"; if(i>1) std::cout<<"^"<<i; }
                    first=false;
                }
                if(first) std::cout<<"0";
            }
    };

    //===== NTT（数论变换）快速多项式乘法（模 998244353=119·2^23+1，原根 3）=====
    //系数为 int 的数组卷积；长度限制：结果长度需 ≤ 2^23
    inline long long _ntt_powmod(long long a,long long b,int mod){
        long long r=1;
        while(b){ if(b&1) r=r*a%mod; a=a*a%mod; b>>=1; }
        return r;
    }
    inline void ntt(std::vector<int>& a,bool invert,int mod=998244353,int root=3){
        int n=(int)a.size();
        for(int i=1,j=0;i<n;i++){
            int bit=n>>1;
            for(;j&bit;bit>>=1) j^=bit;
            j^=bit;
            if(i<j) std::swap(a[i],a[j]);
        }
        for(int len=2;len<=n;len<<=1){
            long long wlen=_ntt_powmod(root,(mod-1)/len,mod);
            if(invert) wlen=_ntt_powmod(wlen,mod-2,mod);
            for(int i=0;i<n;i+=len){
                long long w=1;
                for(int j=0;j<len/2;j++){
                    int u=a[i+j];
                    int v=(int)(a[i+j+len/2]*w%mod);
                    a[i+j]=(u+v<mod)?u+v:u+v-mod;
                    a[i+j+len/2]=(u-v>=0)?u-v:u-v+mod;
                    w=w*wlen%mod;
                }
            }
        }
        if(invert){
            long long inv_n=_ntt_powmod(n,mod-2,mod);
            for(int& x:a) x=(int)(x*inv_n%mod);
        }
    }
    //两个系数数组的卷积（结果系数均取模 mod）
    inline std::vector<int> ntt_multiply(const std::vector<int>& a,const std::vector<int>& b,
                                         int mod=998244353){
        if(a.empty()||b.empty()) return {};
        int need=(int)a.size()+(int)b.size()-1;
        int n=1;
        while(n<need) n<<=1;
        std::vector<int> fa(a.begin(),a.end()),fb(b.begin(),b.end());
        fa.resize(n);fb.resize(n);
        ntt(fa,false,mod);ntt(fb,false,mod);
        for(int i=0;i<n;i++) fa[i]=(int)((long long)fa[i]*fb[i]%mod);
        ntt(fa,true,mod);
        fa.resize(need);
        return fa;
    }

    //===== FFT（double）快速多项式乘法 =====
    //浮点版本：系数大/长度长时精度有限，整数精确卷积请用 ntt_multiply
    inline void fft(std::vector<std::complex<double>>& a,bool invert){
        int n=(int)a.size();
        for(int i=1,j=0;i<n;i++){
            int bit=n>>1;
            for(;j&bit;bit>>=1) j^=bit;
            j^=bit;
            if(i<j) std::swap(a[i],a[j]);
        }
        for(int len=2;len<=n;len<<=1){
            double ang=2.0*std::acos(-1.0)/len*(invert?-1.0:1.0);
            std::complex<double> wlen(std::cos(ang),std::sin(ang));
            for(int i=0;i<n;i+=len){
                std::complex<double> w(1.0,0.0);
                for(int j=0;j<len/2;j++){
                    std::complex<double> u=a[i+j],v=a[i+j+len/2]*w;
                    a[i+j]=u+v;
                    a[i+j+len/2]=u-v;
                    w*=wlen;
                }
            }
        }
        if(invert) for(auto& x:a) x/= (double)n;
    }
    inline std::vector<double> fft_multiply(const std::vector<double>& a,const std::vector<double>& b){
        if(a.empty()||b.empty()) return {};
        int need=(int)a.size()+(int)b.size()-1;
        int n=1;
        while(n<need) n<<=1;
        std::vector<std::complex<double>> fa(a.begin(),a.end()),fb(b.begin(),b.end());
        fa.resize(n);fb.resize(n);
        fft(fa,false);fft(fb,false);
        for(int i=0;i<n;i++) fa[i]*=fb[i];
        fft(fa,true);
        std::vector<double> r(need);
        for(int i=0;i<need;i++) r[i]=fa[i].real();
        return r;
    }

    //===== 模 998244353 意义下的多项式运算（配合 NTT）=====
    inline int _ntt_inv_int(int x,int mod){ return (int)_ntt_powmod(x,mod-2,mod); }
    //多项式逆元：要求 a[0]!=0；返回截断到 n 项（牛顿迭代）
    inline std::vector<int> poly_inv(const std::vector<int>& a,int n,int mod=998244353){
        if(n<=0||a.empty()) return {};
        std::vector<int> g(1,_ntt_inv_int(a[0],mod));
        for(int m=1;m<n;m<<=1){
            int sz=std::min(m*2,n);
            std::vector<int> at(a.begin(),a.begin()+std::min((int)a.size(),sz));
            auto ag=ntt_multiply(at,g,mod);
            ag.resize(sz);
            std::vector<int> t(sz,0);
            t[0]=2;
            for(int i=0;i<sz;i++) t[i]=((long long)t[i]-ag[i]+mod)%mod;//2 - ag
            g=ntt_multiply(g,t,mod);
            g.resize(sz);
        }
        g.resize(n);
        return g;
    }
    //多项式对数：要求 a[0]=1；返回截断 n 项（ln(a)=∫a'/a）
    inline std::vector<int> poly_ln(const std::vector<int>& a,int n,int mod=998244353){
        if(n<=0||a.empty()) return {};
        int dlen=std::min((int)a.size()-1,n-1);
        if(dlen<0) dlen=0;
        std::vector<int> der(dlen);
        for(int i=1;i<(int)a.size()&&i<n;i++) der[i-1]=(long long)a[i]*i%mod;
        auto inv=poly_inv(a,n,mod);
        auto prod=ntt_multiply(der,inv,mod);
        prod.resize(n);
        std::vector<int> res(n,0);
        for(int i=1;i<n;i++) res[i]=(long long)prod[i-1]*_ntt_inv_int(i,mod)%mod;
        return res;
    }
    //多项式指数：要求 a[0]=0；返回截断 n 项（牛顿迭代 g=g*(1-ln g+a)）
    inline std::vector<int> poly_exp(const std::vector<int>& a,int n,int mod=998244353){
        if(n<=0) return {};
        std::vector<int> g(1,1);
        for(int m=1;m<n;m<<=1){
            int sz=std::min(m*2,n);
            auto lg=poly_ln(g,sz,mod);
            std::vector<int> t(sz,0);
            for(int i=0;i<sz;i++){
                long long ai=(i<(int)a.size())?a[i]:0;
                long long li=(i<(int)lg.size())?lg[i]:0;
                t[i]=(int)((ai-li)%mod+mod)%mod;
            }
            t[0]=(t[0]+1)%mod;//1-ln g+a
            g=ntt_multiply(g,t,mod);
            g.resize(sz);
        }
        g.resize(n);
        return g;
    }
}
