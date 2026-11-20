#ifndef BETTER_STD_STD_SIMPLEX_HPP
#define BETTER_STD_STD_SIMPLEX_HPP

//===== better_std 单纯形法（线性规划）=====
//   求解 max c^T x，约束 Ax <= b（b>=0），x >= 0。
//   返回 {最优值, 一组最优解 x, 是否可行有界}。
//   浮点实现，精度按 double；适合中小规模。
#include<vector>
#include<cstddef>
#include<limits>
#include<cmath>
namespace better_std{

struct simplex_result{
    double value=0;
    std::vector<double> x;
    bool feasible=false;
    bool bounded=true;
};

// A: m×n；b: m；c: n（最大化）
inline simplex_result simplex(const std::vector<std::vector<double>>& A,
                              const std::vector<double>& b,
                              const std::vector<double>& c){
    int m=(int)A.size(), n=(int)c.size();
    simplex_result res;
    if(m==0||n==0){ res.feasible=true; res.x.assign(n,0); return res; }
    const double EPS=1e-9;
    //  tableau: (m+1) × (n+m+1)，末尾列为 RHS；松弛变量 n..n+m-1
    int W=n+m+1;
    std::vector<std::vector<double>> t(m+1,std::vector<double>(W,0));
    for(int i=0;i<m;i++){
        for(int j=0;j<n;j++) t[i][j]=A[i][j];
        t[i][n+i]=1;             // 松弛变量
        t[i][W-1]=b[i];
    }
    for(int j=0;j<n;j++) t[m][j]=-c[j];  // 目标行（最小化 -z）
    std::vector<int> basis(m);
    for(int i=0;i<m;i++) basis[i]=n+i;

    for(int iter=0; iter<100000; iter++){
        // 选入基变量（目标行最小负系数）
        int enter=-1;
        for(int j=0;j<W-1;j++) if(t[m][j] < -EPS && (enter<0||t[m][j]<t[m][enter])) enter=j;
        if(enter<0) break;       // 最优
        // 选出基变量（最小比值）
        int leave=-1; double best=std::numeric_limits<double>::infinity();
        for(int i=0;i<m;i++) if(t[i][enter]>EPS){
            double ratio=t[i][W-1]/t[i][enter];
            if(ratio<best-EPS || (std::fabs(ratio-best)<=EPS && (leave<0||basis[i]<basis[leave]))){ best=ratio; leave=i; }
        }
        if(leave<0){ res.bounded=false; return res; }   // 无界
        // 主元消去
        double pv=t[leave][enter];
        for(int j=0;j<W;j++) t[leave][j]/=pv;
        for(int i=0;i<=m;i++) if(i!=leave){
            double f=t[i][enter];
            if(std::fabs(f)<EPS) continue;
            for(int j=0;j<W;j++) t[i][j]-=f*t[leave][j];
        }
        basis[leave]=enter;
    }
    res.feasible=true;
    res.value=t[m][W-1];
    res.x.assign(n,0);
    for(int i=0;i<m;i++) if(basis[i]<n) res.x[basis[i]]=t[i][W-1];
    return res;
}

} // namespace better_std
#endif // BETTER_STD_STD_SIMPLEX_HPP
