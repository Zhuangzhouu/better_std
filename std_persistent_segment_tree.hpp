#pragma once
#include<vector>
#include<algorithm>
namespace better_std{
    //可持久化线段树（主席树）：区间第 k 小
    //构造时对数组离散化并建立版本根（roots[i] = a[0..i-1] 的版本）
    template<typename T>
    class std_persistent_segment_tree{
        private:
            struct Node{ int l=-1,r=-1,cnt=0; };
            std::vector<Node> pool;
            std::vector<T> vals;//离散化后的有序值
            std::vector<int> roots;//版本根
            int new_node(){ pool.push_back(Node()); return (int)pool.size()-1; }
            int build_empty(int lo,int hi){
                int p=new_node();
                if(lo==hi) return p;
                int mid=(lo+hi)/2;
                pool[p].l=build_empty(lo,mid);
                pool[p].r=build_empty(mid+1,hi);
                return p;
            }
            int update(int p,int lo,int hi,int pos){
                int q=new_node();
                pool[q]=pool[p];
                if(lo==hi){ pool[q].cnt++; return q; }
                int mid=(lo+hi)/2;
                if(pos<=mid) pool[q].l=update(pool[p].l,lo,mid,pos);
                else pool[q].r=update(pool[p].r,mid+1,hi,pos);
                pool[q].cnt=pool[pool[q].l].cnt+pool[pool[q].r].cnt;
                return q;
            }
            int kth(int a,int b,int lo,int hi,int k) const{
                if(lo==hi) return lo;
                int mid=(lo+hi)/2;
                int left=pool[pool[b].l].cnt-pool[pool[a].l].cnt;
                if(k<=left) return kth(pool[a].l,pool[b].l,lo,mid,k);
                return kth(pool[a].r,pool[b].r,mid+1,hi,k-left);
            }
        public:
            std_persistent_segment_tree(const std::vector<T>& a){
                vals=a;
                std::sort(vals.begin(),vals.end());
                vals.erase(std::unique(vals.begin(),vals.end()),vals.end());
                int m=(int)vals.size();
                roots.push_back(build_empty(0,m-1));
                int cur=roots[0];
                for(T x:a){
                    int pos=(int)(std::lower_bound(vals.begin(),vals.end(),x)-vals.begin());
                    cur=update(cur,0,m-1,pos);
                    roots.push_back(cur);
                }
            }
            //原数组 [l,r) 的第 k 小（1-based）
            T range_kth(int l,int r,int k) const{
                int idx=kth(roots[l],roots[r],0,(int)vals.size()-1,k);
                return vals[idx];
            }
            int versions() const{ return (int)roots.size(); }
    };
}
