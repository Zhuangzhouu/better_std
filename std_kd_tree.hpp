#ifndef BETTER_STD_STD_KD_TREE_HPP
#define BETTER_STD_STD_KD_TREE_HPP

//===== better_std KD-Tree：k 维点集最近邻 + 矩形范围查询 =====
//   点用 std::vector<T>（T 为数值类型，如 double/int/long long）。
//   API：build(points)、nearest(query) 返回最近点及其距离平方、range_count(lo,hi) 统计矩形内点数。
//   构建：按方差最大维度取中位数（O(n log^2 n)）；查询剪枝用子树包围盒。
#include<vector>
#include<algorithm>
#include<numeric>
#include<limits>
#include<cmath>
#include<utility>
namespace better_std{

template<typename T>
class kd_tree{
    using point=std::vector<T>;
    struct node{
        point p;
        std::vector<T> lo,hi;       // 子树包围盒
        int l,r;
        node(const point& p_):p(p_),lo(p_),hi(p_),l(-1),r(-1){}
    };
    std::vector<node> tr;
    int dim_, root_=-1;

    int build_(std::vector<point> pts,int l,int r){
        if(l>=r) return -1;
        // 选择方差最大的维度
        int best=0; long long bestvar=-1;
        for(int d=0;d<dim_;d++){
            long long sum=0,sq=0;
            for(int i=l;i<r;i++){ long long v=(long long)pts[i][d]; sum+=v; sq+=v*v; }
            long long var=sq*r-sum*sum;
            if(var>bestvar){ bestvar=var; best=d; }
        }
        int mid=(l+r)/2;
        // 用 sort 保证分区确定（nth_element 对不同 -std 下相等元素的分区可能不同）
        std::sort(pts.begin()+l,pts.begin()+r,
                  [&](const point& a,const point& b){
                      if(a[best]!=b[best]) return a[best]<b[best];
                      for(int d=0;d<dim_;d++) if(a[d]!=b[d]) return a[d]<b[d];
                      return false;
                  });
        int id=(int)tr.size();
        tr.push_back(node(pts[mid]));
        // 先算子树索引再赋值：build_ 内 push_back 会重分配 tr，赋值左右求值顺序未指定会悬垂
        int lc=build_(pts,l,mid);
        int rc=build_(pts,mid+1,r);
        tr[id].l=lc;
        tr[id].r=rc;
        for(int d=0;d<dim_;d++){
            if(tr[id].l>=0){ tr[id].lo[d]=std::min(tr[id].lo[d],tr[tr[id].l].lo[d]); tr[id].hi[d]=std::max(tr[id].hi[d],tr[tr[id].l].hi[d]); }
            if(tr[id].r>=0){ tr[id].lo[d]=std::min(tr[id].lo[d],tr[tr[id].r].lo[d]); tr[id].hi[d]=std::max(tr[id].hi[d],tr[tr[id].r].hi[d]); }
        }
        return id;
    }
    static long long dist2_(const point& a,const point& b){
        long long s=0;
        for(size_t i=0;i<a.size();i++){ long long d=(long long)a[i]-b[i]; s+=d*d; }
        return s;
    }
    // 查询点到矩形盒的最小距离平方（剪枝下界）
    static long long boxdist2_(const point& q,const std::vector<T>& lo,const std::vector<T>& hi){
        long long s=0;
        for(size_t i=0;i<q.size();i++){
            long long v=(long long)q[i];
            if(v<(long long)lo[i]) s+=((long long)lo[i]-v)*((long long)lo[i]-v);
            else if(v>(long long)hi[i]) s+=(v-(long long)hi[i])*(v-(long long)hi[i]);
        }
        return s;
    }
    void nearest_(int u,const point& q,long long& best,point& bestp) const{
        if(u<0) return;
        long long d=dist2_(tr[u].p,q);
        if(d<best){ best=d; bestp=tr[u].p; }
        int l=tr[u].l,r=tr[u].r;
        long long dl=l>=0?boxdist2_(q,tr[l].lo,tr[l].hi):std::numeric_limits<long long>::max();
        long long dr=r>=0?boxdist2_(q,tr[r].lo,tr[r].hi):std::numeric_limits<long long>::max();
        if(dl<dr){ if(dl<best) nearest_(l,q,best,bestp); if(dr<best) nearest_(r,q,best,bestp); }
        else { if(dr<best) nearest_(r,q,best,bestp); if(dl<best) nearest_(l,q,best,bestp); }
    }
    void range_(int u,const point& lo,const point& hi,int& cnt) const{
        if(u<0) return;
        const point& p=tr[u].p;
        bool inside=true;
        for(int d=0;d<dim_;d++) if(p[d]<lo[d]||p[d]>hi[d]){ inside=false; break; }
        if(inside) cnt++;
        if(tr[u].l>=0){
            bool ok=true;
            for(int d=0;d<dim_;d++) if(tr[tr[u].l].hi[d]<lo[d]||tr[tr[u].l].lo[d]>hi[d]){ ok=false; break; }
            if(ok) range_(tr[u].l,lo,hi,cnt);
        }
        if(tr[u].r>=0){
            bool ok=true;
            for(int d=0;d<dim_;d++) if(tr[tr[u].r].hi[d]<lo[d]||tr[tr[u].r].lo[d]>hi[d]){ ok=false; break; }
            if(ok) range_(tr[u].r,lo,hi,cnt);
        }
    }
public:
    kd_tree() {}
    explicit kd_tree(const std::vector<point>& pts){ build(pts); }
    void build(const std::vector<point>& pts){
        tr.clear(); root_=-1;
        if(pts.empty()) return;
        dim_=(int)pts[0].size();
        root_=build_(pts,0,(int)pts.size());
    }
    bool empty() const { return root_<0; }
    size_t size() const { return tr.size(); }
    // 返回 {最近点, 距离平方}
    std::pair<point,long long> nearest(const point& q) const{
        point bestp;
        long long best=std::numeric_limits<long long>::max();
        if(root_>=0) nearest_(root_,q,best,bestp);
        return {bestp,best};
    }
    // 统计矩形 [lo,hi] 内的点数（闭区间）
    int range_count(const point& lo,const point& hi) const{
        int cnt=0;
        if(root_>=0) range_(root_,lo,hi,cnt);
        return cnt;
    }
};

} // namespace better_std
#endif // BETTER_STD_STD_KD_TREE_HPP
