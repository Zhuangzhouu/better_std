#ifndef BETTER_STD_STD_SQRT_DECOMPOSITION_HPP
#define BETTER_STD_STD_SQRT_DECOMPOSITION_HPP

#include<vector>
#include<algorithm>
#include<cmath>
#include<climits>
namespace better_std{
    //===== 分块（根号分解）：区间加 / 区间和 / 区间最大值 / 区间内 <=x 计数 =====
    //  raw[i] = 实际值 - tag[bel(i)]；整块加只改 tag，块内排序数组不含 tag
    //  复杂度：O((n/B)+B) 每次操作，B≈sqrt(n)
    class sqrt_decomposition{
        int n_=0,B_=1,nb_=0;
        std::vector<long long> raw_,tag_,sum_,mx_;
        std::vector<std::vector<long long>> sorted_;
        int bel(int i)const{ return i/B_; }
        int L(int b)const{ return b*B_; }
        int R(int b)const{ return std::min(n_,(b+1)*B_)-1; }
        void rebuild(int b){
            if(b<0||b>=nb_) return;
            long long s=0,m=LLONG_MIN;
            sorted_[b].clear();
            for(int i=L(b);i<=R(b);i++){
                s+=raw_[i]; if(raw_[i]>m) m=raw_[i];
                sorted_[b].push_back(raw_[i]);
            }
            sum_[b]=s; mx_[b]=m;
            std::sort(sorted_[b].begin(),sorted_[b].end());
        }
    public:
        sqrt_decomposition()=default;
        explicit sqrt_decomposition(const std::vector<long long>& a){ build(a); }
        void build(const std::vector<long long>& a){
            raw_=a; n_=(int)a.size();
            B_=std::max(1,(int)std::sqrt((double)std::max(1,n_))+1);
            nb_=(n_+B_-1)/B_;
            tag_.assign(nb_,0); sum_.assign(nb_,0); mx_.assign(nb_,LLONG_MIN);
            sorted_.assign(nb_,{});
            for(int b=0;b<nb_;b++) rebuild(b);
        }
        int size()const{ return n_; }
        long long point_query(int i)const{ return raw_[i]+tag_[bel(i)]; }
        std::vector<long long> to_vector()const{
            std::vector<long long> v(n_);
            for(int i=0;i<n_;i++) v[i]=point_query(i);
            return v;
        }
        void range_add(int l,int r,long long d){
            if(n_==0||l>r) return;
            l=std::max(l,0); r=std::min(r,n_-1);
            if(l>r) return;
            int bl=bel(l),br=bel(r);
            if(bl==br){
                for(int i=l;i<=r;i++) raw_[i]+=d;
                rebuild(bl);
                return;
            }
            for(int i=l;i<=R(bl);i++) raw_[i]+=d;
            rebuild(bl);
            for(int i=L(br);i<=r;i++) raw_[i]+=d;
            rebuild(br);
            for(int b=bl+1;b<br;b++) tag_[b]+=d;
        }
        long long range_sum(int l,int r)const{
            if(n_==0||l>r) return 0;
            l=std::max(l,0); r=std::min(r,n_-1);
            if(l>r) return 0;
            int bl=bel(l),br=bel(r);
            long long ans=0;
            if(bl==br){ for(int i=l;i<=r;i++) ans+=raw_[i]+tag_[bl]; return ans; }
            for(int i=l;i<=R(bl);i++) ans+=raw_[i]+tag_[bl];
            for(int i=L(br);i<=r;i++) ans+=raw_[i]+tag_[br];
            for(int b=bl+1;b<br;b++) ans+=sum_[b]+tag_[b]*(R(b)-L(b)+1);
            return ans;
        }
        long long range_max(int l,int r)const{
            if(n_==0||l>r) return LLONG_MIN;
            l=std::max(l,0); r=std::min(r,n_-1);
            if(l>r) return LLONG_MIN;
            int bl=bel(l),br=bel(r);
            long long ans=LLONG_MIN;
            if(bl==br){ for(int i=l;i<=r;i++) ans=std::max(ans,raw_[i]+tag_[bl]); return ans; }
            for(int i=l;i<=R(bl);i++) ans=std::max(ans,raw_[i]+tag_[bl]);
            for(int i=L(br);i<=r;i++) ans=std::max(ans,raw_[i]+tag_[br]);
            for(int b=bl+1;b<br;b++) ans=std::max(ans,mx_[b]+tag_[b]);
            return ans;
        }
        //区间内 <=x 的元素个数
        int count_leq(int l,int r,long long x)const{
            if(n_==0||l>r) return 0;
            l=std::max(l,0); r=std::min(r,n_-1);
            if(l>r) return 0;
            int bl=bel(l),br=bel(r);
            int ans=0;
            if(bl==br){ for(int i=l;i<=r;i++) if(raw_[i]+tag_[bl]<=x) ans++; return ans; }
            for(int i=l;i<=R(bl);i++) if(raw_[i]+tag_[bl]<=x) ans++;
            for(int i=L(br);i<=r;i++) if(raw_[i]+tag_[br]<=x) ans++;
            for(int b=bl+1;b<br;b++){
                long long lim=x-tag_[b];
                ans+=(int)(std::upper_bound(sorted_[b].begin(),sorted_[b].end(),lim)-sorted_[b].begin());
            }
            return ans;
        }
    };
}

#endif // BETTER_STD_STD_SQRT_DECOMPOSITION_HPP
