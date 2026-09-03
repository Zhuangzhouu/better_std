#pragma once
#include<vector>
#include<utility>
namespace better_std{
    //并查集：路径压缩 + 按大小合并（近似常数时间）
    //find/merge 非 const（会改内部结构）；same/size/count 供查询
    class std_dsu{
        private:
            int n_,cnt_;
            std::vector<int> fa_,sz_;
            int find_(int x){
                while(fa_[x]!=x){ fa_[x]=fa_[fa_[x]]; x=fa_[x]; }
                return x;
            }
        public:
            std_dsu(int n=0):n_(n),cnt_(n),fa_(n),sz_(n,1){
                for(int i=0;i<n;i++) fa_[i]=i;
            }
            void reset(int n){
                n_=n;cnt_=n;
                fa_.resize(n);sz_.assign(n,1);
                for(int i=0;i<n;i++) fa_[i]=i;
            }
            int find(int x){ return find_(x); }
            //合并 a、b 所在集合；原来不同集合时返回 true
            bool merge(int a,int b){
                a=find_(a);b=find_(b);
                if(a==b) return false;
                if(sz_[a]<sz_[b]) std::swap(a,b);
                fa_[b]=a;sz_[a]+=sz_[b];cnt_--;
                return true;
            }
            bool same(int a,int b){ return find_(a)==find_(b); }
            int size(int x){ return sz_[find_(x)]; }//x 所在集合大小
            int count() const{ return cnt_; }//当前集合个数
            std::vector<int> roots() const{//所有集合代表元（按编号升序）
                std::vector<int> r;
                for(int i=0;i<n_;i++) if(fa_[i]==i) r.push_back(i);
                return r;
            }
    };
}
