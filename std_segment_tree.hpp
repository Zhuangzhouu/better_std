#pragma once
#include<iostream>
#include<vector>
#include<algorithm>
#include<limits>
namespace better_std{
    //线段树：区间加（懒标记）、区间赋值（懒标记，优先级高于加）、区间和、区间最大值
    //递归实现，4n 数组；下标 0-based
    template<typename T=long long>
    class std_segment_tree{
        private:
            int n_;
            mutable std::vector<T> sum_,mx_,lazy_add_,set_val_;
            mutable std::vector<char> has_set_;//懒标记传播在 const 查询中也会发生
            void build(int p,int l,int r,const std::vector<T>& a){
                if(l==r){ sum_[p]=mx_[p]=a[l]; return; }
                int m=(l+r)/2;
                build(p*2,l,m,a);
                build(p*2+1,m+1,r,a);
                sum_[p]=sum_[p*2]+sum_[p*2+1];
                mx_[p]=std::max(mx_[p*2],mx_[p*2+1]);
            }
            void apply_set(int p,int l,int r,const T& v) const{
                sum_[p]=v*(T)(r-l+1);
                mx_[p]=v;
                has_set_[p]=1;
                set_val_[p]=v;
                lazy_add_[p]=T();//赋值覆盖之前的加
            }
            void apply_add(int p,int l,int r,const T& v) const{
                sum_[p]+=v*(T)(r-l+1);
                mx_[p]+=v;
                lazy_add_[p]+=v;
            }
            void push(int p,int l,int r) const{
                if(l==r) return;
                int m=(l+r)/2;
                if(has_set_[p]){
                    apply_set(p*2,l,m,set_val_[p]);
                    apply_set(p*2+1,m+1,r,set_val_[p]);
                    has_set_[p]=0;
                }
                if(lazy_add_[p]!=T()){
                    apply_add(p*2,l,m,lazy_add_[p]);
                    apply_add(p*2+1,m+1,r,lazy_add_[p]);
                    lazy_add_[p]=T();
                }
            }
            void upd(int p){
                sum_[p]=sum_[p*2]+sum_[p*2+1];
                mx_[p]=std::max(mx_[p*2],mx_[p*2+1]);
            }
            void range_add(int p,int l,int r,int ql,int qr,const T& v){
                if(ql<=l&&r<=qr){ apply_add(p,l,r,v); return; }
                push(p,l,r);
                int m=(l+r)/2;
                if(ql<=m) range_add(p*2,l,m,ql,qr,v);
                if(qr>m)  range_add(p*2+1,m+1,r,ql,qr,v);
                upd(p);
            }
            void range_set(int p,int l,int r,int ql,int qr,const T& v){
                if(ql<=l&&r<=qr){ apply_set(p,l,r,v); return; }
                push(p,l,r);
                int m=(l+r)/2;
                if(ql<=m) range_set(p*2,l,m,ql,qr,v);
                if(qr>m)  range_set(p*2+1,m+1,r,ql,qr,v);
                upd(p);
            }
            T qsum(int p,int l,int r,int ql,int qr) const{
                if(ql<=l&&r<=qr) return sum_[p];
                push(p,l,r);
                int m=(l+r)/2;
                T s=T();
                if(ql<=m) s+=qsum(p*2,l,m,ql,qr);
                if(qr>m)  s+=qsum(p*2+1,m+1,r,ql,qr);
                return s;
            }
            T qmax(int p,int l,int r,int ql,int qr) const{
                if(ql<=l&&r<=qr) return mx_[p];
                push(p,l,r);
                int m=(l+r)/2;
                T res=std::numeric_limits<T>::lowest();
                if(ql<=m) res=std::max(res,qmax(p*2,l,m,ql,qr));
                if(qr>m)  res=std::max(res,qmax(p*2+1,m+1,r,ql,qr));
                return res;
            }
        public:
            std_segment_tree(int n=0):n_(n),
                sum_(4*n+4,T()),mx_(4*n+4,T()),
                lazy_add_(4*n+4,T()),set_val_(4*n+4,T()),
                has_set_(4*n+4,0){}
            void build(const std::vector<T>& a){
                n_=(int)a.size();
                sum_.assign(4*n_+4,T());
                mx_.assign(4*n_+4,T());
                lazy_add_.assign(4*n_+4,T());
                set_val_.assign(4*n_+4,T());
                has_set_.assign(4*n_+4,0);
                if(n_>0) build(1,0,n_-1,a);
            }
            void add(int l,int r,const T& v){ if(l<=r) range_add(1,0,n_-1,l,r,v); }
            void set(int l,int r,const T& v){ if(l<=r) range_set(1,0,n_-1,l,r,v); }
            void set_point(int i,const T& v){ set(i,i,v); }
            T range_sum(int l,int r) const{ return (l<=r)?qsum(1,0,n_-1,l,r):T(); }
            T range_max(int l,int r) const{ return (l<=r)?qmax(1,0,n_-1,l,r):std::numeric_limits<T>::lowest(); }
            int size() const{ return n_; }
    };
}
