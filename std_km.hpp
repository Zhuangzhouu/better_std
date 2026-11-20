#ifndef BETTER_STD_STD_KM_HPP
#define BETTER_STD_STD_KM_HPP

#include<vector>
#include<algorithm>
#include<climits>
namespace better_std{
    //===== KM 算法：二分图最大权「完美匹配」（左部 nL 个点必须全部匹配，要求 nL<=nR）=====
    //  w[i][j] 为权值，用 NEG_INF 表示无边；可为负权。O(nL^2 * nR)
    //  返回 feasible=false 表示不存在覆盖全部左部点的匹配
    struct km_result{
        bool feasible=false;
        long long value=0;
        std::vector<int> matchL;  //matchL[i]=匹配的右部点下标
    };
    struct km_matcher{
        static constexpr long long NEG_INF=-(1LL<<50);
        int nL=0,nR=0;
        std::vector<std::vector<long long>> w;
        std::vector<long long> lx,ly,slack;
        std::vector<int> matchX,matchY,pre;
        std::vector<char> visX,visY;
        km_matcher()=default;
        km_matcher(const std::vector<std::vector<long long>>& weight,int L,int R){ init(weight,L,R); }
        void init(const std::vector<std::vector<long long>>& weight,int L,int R){
            nL=L; nR=R; w=weight;
            lx.assign(nL,NEG_INF); ly.assign(nR,0); slack.assign(nR,0);
            matchX.assign(nL,-1); matchY.assign(nR,-1); pre.assign(nR,-1);
            visX.assign(nL,0); visY.assign(nR,0);
        }
        bool dfs(int u){
            visX[u]=1;
            for(int v=0;v<nR;v++){
                if(visY[v]||w[u][v]==NEG_INF) continue;  //无边不可匹配，顶标调整后也不得误选
                long long gap=lx[u]+ly[v]-w[u][v];
                if(gap==0){
                    visY[v]=1;
                    if(matchY[v]==-1||dfs(matchY[v])){
                        matchX[u]=v; matchY[v]=u; return true;
                    }
                }else if(gap<slack[v]) slack[v]=gap;
            }
            return false;
        }
        km_result solve(){
            km_result res; res.matchL.assign(nL,-1);
            if(nL<=0||nR<nL) return res;
            for(int i=0;i<nL;i++){
                for(int j=0;j<nR;j++) if(w[i][j]>lx[i]) lx[i]=w[i][j];
                if(lx[i]==NEG_INF) return res;  //该行无任何可行边
            }
            const long long INF=LLONG_MAX/4;
            for(int u=0;u<nL;u++){
                while(true){
                    std::fill(visX.begin(),visX.end(),0);
                    std::fill(visY.begin(),visY.end(),0);
                    std::fill(slack.begin(),slack.end(),INF);
                    if(dfs(u)) break;
                    long long d=INF;
                    for(int v=0;v<nR;v++) if(!visY[v]&&slack[v]<d) d=slack[v];
                    if(d>=INF/2) return res;  //不存在完美匹配
                    for(int i=0;i<nL;i++) if(visX[i]) lx[i]-=d;
                    for(int v=0;v<nR;v++) if(visY[v]) ly[v]+=d;
                }
            }
            res.feasible=true;
            for(int i=0;i<nL;i++){
                res.matchL[i]=matchX[i];
                if(matchX[i]>=0) res.value+=w[i][matchX[i]];
                else { res.feasible=false; res.value=0; res.matchL.assign(nL,-1); return res; }
            }
            return res;
        }
    };
    inline km_result km_max_weight(const std::vector<std::vector<long long>>& w,int nL,int nR){
        km_matcher km(w,nL,nR);
        return km.solve();
    }
    //最小权完美匹配：取负后跑最大权（注意 NEG_INF 不能用 LLONG_MIN 取负）
    inline km_result km_min_weight(const std::vector<std::vector<long long>>& w,int nL,int nR){
        std::vector<std::vector<long long>> neg(nL,std::vector<long long>(nR,km_matcher::NEG_INF));
        for(int i=0;i<nL;i++) for(int j=0;j<nR;j++)
            if(w[i][j]!=km_matcher::NEG_INF) neg[i][j]=-w[i][j];
        km_result r=km_max_weight(neg,nL,nR);
        if(r.feasible) r.value=-r.value;
        return r;
    }
}

#endif // BETTER_STD_STD_KM_HPP
