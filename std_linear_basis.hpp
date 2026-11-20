#ifndef BETTER_STD_STD_LINEAR_BASIS_HPP
#define BETTER_STD_STD_LINEAR_BASIS_HPP

#include<vector>
#include<algorithm>
#include<cstdint>
namespace better_std{
    //===== 异或线性基（unsigned long long，位 0..62）=====
    //  内部维护行简化阶梯：b[i] 的最高位为 i，且其他基在第 i 位均为 0
    //  insert / can / max_xor / min_nonzero / kth_smallest / merge 均 O(63)
    struct xor_linear_basis{
        static constexpr int B=62;
        unsigned long long b[B+1]{};
        void clear(){ for(int i=0;i<=B;i++) b[i]=0; }
        //插入 x：返回是否增加了秩（即 x 不能被现有基表示）
        bool insert(unsigned long long x){
            for(int i=B;i>=0;i--){
                if(!((x>>i)&1ull)) continue;
                if(!b[i]){
                    b[i]=x;
                    for(int j=i-1;j>=0;j--) if((b[i]>>j)&1ull) b[i]^=b[j];
                    for(int j=i+1;j<=B;j++) if((b[j]>>i)&1ull) b[j]^=b[i];
                    return true;
                }
                x^=b[i];
            }
            return false;
        }
        bool can(unsigned long long x)const{
            for(int i=B;i>=0;i--){
                if(!((x>>i)&1ull)) continue;
                if(!b[i]) return false;
                x^=b[i];
            }
            return true;
        }
        int rank()const{ int r=0; for(int i=0;i<=B;i++) if(b[i]) r++; return r; }
        unsigned long long max_xor(unsigned long long init=0)const{
            for(int i=B;i>=0;i--) if((init^b[i])>init) init^=b[i];
            return init;
        }
        unsigned long long min_nonzero_xor()const{
            for(int i=0;i<=B;i++) if(b[i]) return b[i];
            return 0;  //秩为 0 时无非零可表示值
        }
        //第 k 小可表示值（1-based，k=1 → 0，空集）；k 越界返回 false
        bool kth_smallest(unsigned long long k,unsigned long long& out)const{
            std::vector<unsigned long long> v;
            v.reserve(B+1);
            for(int i=0;i<=B;i++) if(b[i]) v.push_back(b[i]);  //主元升序
            if(k==0||k>(1ull<<v.size())) return false;
            unsigned long long m=k-1, ans=0;
            for(size_t j=0;j<v.size();j++) if((m>>j)&1ull) ans^=v[j];
            out=ans; return true;
        }
        void merge(const xor_linear_basis& o){
            for(int i=0;i<=B;i++) if(o.b[i]) insert(o.b[i]);
        }
    };

    //===== 前缀线性基（带位置）：支持区间 [l,r] 最大异或（1-based 位置）=====
    //  每个主元位保留「位置最靠右」的基；query(l,r) 只用 pos>=l 的基
    struct prefix_linear_basis{
        static constexpr int B=62;
        struct Slot{ unsigned long long v=0; int pos=-1; };
        int n=0;
        std::vector<std::vector<Slot>> pref;
        static void insert_at(std::vector<Slot>& bs,unsigned long long v,int p){
            int pos=p;
            for(int j=B;j>=0;j--){
                if(!((v>>j)&1ull)) continue;
                if(bs[j].v==0){ bs[j].v=v; bs[j].pos=pos; return; }
                if(bs[j].pos<pos){ std::swap(bs[j].v,v); std::swap(bs[j].pos,pos); }
                v^=bs[j].v;
            }
        }
        void build(const std::vector<unsigned long long>& a){
            n=(int)a.size();
            pref.assign(n+1,std::vector<Slot>(B+1));
            for(int i=1;i<=n;i++){
                pref[i]=pref[i-1];
                insert_at(pref[i],a[i-1],i);
            }
        }
        //区间 [l,r]（1-based 闭区间）内选一个子集，使 init^子集异或 最大
        unsigned long long query(int l,int r,unsigned long long init=0)const{
            if(l<1) l=1;
            if(r>n) r=n;
            if(l>r||n==0) return init;
            for(int j=B;j>=0;j--){
                const Slot& s=pref[r][j];
                if(s.pos>=l&&(init^s.v)>init) init^=s.v;
            }
            return init;
        }
    };
}

#endif // BETTER_STD_STD_LINEAR_BASIS_HPP
