#pragma once
#include<vector>
#include<algorithm>
#include<limits>
namespace better_std{
    //SegTree Beats：区间取 min（chmin）+ 区间加 + 区间和/区间最大值
    //每节点维护 max1（最大值）/ max2（严格次大）/ maxc（最大值个数）/ add 懒标记
    //chmin(x)：当 x∈(max2, max1] 时可整点更新（sum -= (max1-x)*maxc, max1=x），否则下推
    template<typename T=long long>
    class std_segment_tree_beats{
        private:
            int n_;
            const T NEG;
            struct Node{ T sum=0,max1=0,max2=0,add=0; int maxc=1; };
            mutable std::vector<Node> tr_;
            void build(int p,int l,int r,const std::vector<T>& a){
                if(l==r){
                    tr_[p].sum=tr_[p].max1=a[l];
                    tr_[p].max2=NEG;
                    tr_[p].maxc=1;
                    return;
                }
                int m=(l+r)/2;
                build(p*2,l,m,a);
                build(p*2+1,m+1,r,a);
                pull(p);
            }
            void pull(int p){
                Node& x=tr_[p];const Node& L=tr_[p*2];const Node& R=tr_[p*2+1];
                x.sum=L.sum+R.sum;
                x.add=T();
                if(L.max1==R.max1){
                    x.max1=L.max1;
                    x.maxc=L.maxc+R.maxc;
                    x.max2=std::max(L.max2,R.max2);
                }else if(L.max1>R.max1){
                    x.max1=L.max1;
                    x.maxc=L.maxc;
                    x.max2=std::max(L.max2,R.max1);
                }else{
                    x.max1=R.max1;
                    x.maxc=R.maxc;
                    x.max2=std::max(L.max1,R.max2);
                }
            }
            void apply_add(int p,int l,int r,const T& v) const{
                tr_[p].sum+=v*(T)(r-l+1);
                tr_[p].max1+=v;
                tr_[p].max2+=v;
                tr_[p].add+=v;
            }
            void apply_chmin(int p,const T& x) const{
                if(tr_[p].max1<=x) return;
                tr_[p].sum-=(tr_[p].max1-x)*(T)tr_[p].maxc;
                tr_[p].max1=x;
            }
            void push(int p,int l,int r) const{
                if(l==r) return;
                int m=(l+r)/2;
                if(tr_[p].add!=T()){
                    apply_add(p*2,l,m,tr_[p].add);
                    apply_add(p*2+1,m+1,r,tr_[p].add);
                    tr_[p].add=T();
                }
                apply_chmin(p*2,tr_[p].max1);
                apply_chmin(p*2+1,tr_[p].max1);
            }
            void range_chmin(int p,int l,int r,int ql,int qr,const T& x){
                if(tr_[p].max1<=x) return;
                if(ql<=l&&r<=qr && tr_[p].max2<x){ apply_chmin(p,x); return; }
                push(p,l,r);
                int m=(l+r)/2;
                if(ql<=m) range_chmin(p*2,l,m,ql,qr,x);
                if(qr>m)  range_chmin(p*2+1,m+1,r,ql,qr,x);
                pull(p);
            }
            void range_add(int p,int l,int r,int ql,int qr,const T& v){
                if(ql<=l&&r<=qr){ apply_add(p,l,r,v); return; }
                push(p,l,r);
                int m=(l+r)/2;
                if(ql<=m) range_add(p*2,l,m,ql,qr,v);
                if(qr>m)  range_add(p*2+1,m+1,r,ql,qr,v);
                pull(p);
            }
            T qsum(int p,int l,int r,int ql,int qr) const{
                if(ql<=l&&r<=qr) return tr_[p].sum;
                push(p,l,r);
                int m=(l+r)/2;
                T s=T();
                if(ql<=m) s+=qsum(p*2,l,m,ql,qr);
                if(qr>m)  s+=qsum(p*2+1,m+1,r,ql,qr);
                return s;
            }
            T qmax(int p,int l,int r,int ql,int qr) const{
                if(ql<=l&&r<=qr) return tr_[p].max1;
                push(p,l,r);
                int m=(l+r)/2;
                T res=NEG;
                if(ql<=m) res=std::max(res,qmax(p*2,l,m,ql,qr));
                if(qr>m)  res=std::max(res,qmax(p*2+1,m+1,r,ql,qr));
                return res;
            }
        public:
            std_segment_tree_beats(int n=0)
                :n_(n),NEG(std::numeric_limits<T>::lowest()/2),
                 tr_(4*std::max(n,1)+4){}
            void build(const std::vector<T>& a){
                n_=(int)a.size();
                tr_.assign(4*std::max(n_,1)+4,Node());
                if(n_>0) build(1,0,n_-1,a);
            }
            void chmin(int l,int r,const T& x){ if(l<=r) range_chmin(1,0,n_-1,l,r,x); }
            void add(int l,int r,const T& v){ if(l<=r) range_add(1,0,n_-1,l,r,v); }
            T range_sum(int l,int r) const{ return (l<=r)?qsum(1,0,n_-1,l,r):T(); }
            T range_max(int l,int r) const{ return (l<=r)?qmax(1,0,n_-1,l,r):NEG; }
            int size() const{ return n_; }
    };
}
