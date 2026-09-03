#pragma once
#include<string>
#include<vector>
#include<queue>
#include<algorithm>
namespace better_std{
    //AC 自动机（多模式串匹配）：trie + 失配指针
    //insert 模式串 → build 建 fail（BFS，nxt 补全为 fail 跳转）→ match(text) 返回每个模式出现次数
    //字符集默认 26 个小写字母
    class std_ac_automaton{
        private:
            struct Node{
                int nxt[26];
                int fail=0;
                int out=-1;//以该节点结尾的模式 id（-1 无）
                Node(){ for(int i=0;i<26;i++) nxt[i]=-1; }
            };
            std::vector<Node> pool;
            std::vector<int> pat_node,pat_len_;//模式 id → 结尾节点 / 模式长度
            int idx(char c) const{ return (int)c-'a'; }
        public:
            std_ac_automaton(){ pool.emplace_back(); }
            void insert(const std::string& s){
                int u=0;
                for(char c:s){
                    int i=idx(c);
                    if(pool[u].nxt[i]==-1){
                        pool[u].nxt[i]=(int)pool.size();
                        pool.emplace_back();
                    }
                    u=pool[u].nxt[i];
                }
                pat_node.push_back(u);
                pat_len_.push_back((int)s.size());
                pool[u].out=(int)pat_node.size()-1;
            }
            //BFS 建失配指针；返回 BFS 序（供 match 逆序累加）
            void build(){
                std::vector<int> order;
                std::queue<int> q;
                for(int i=0;i<26;i++){
                    if(pool[0].nxt[i]!=-1){
                        pool[pool[0].nxt[i]].fail=0;
                        q.push(pool[0].nxt[i]);
                        order.push_back(pool[0].nxt[i]);
                    }else pool[0].nxt[i]=0;
                }
                while(!q.empty()){
                    int u=q.front();q.pop();
                    for(int i=0;i<26;i++){
                        int v=pool[u].nxt[i];
                        if(v!=-1){
                            pool[v].fail=pool[pool[u].fail].nxt[i];
                            q.push(v);
                            order.push_back(v);
                        }else pool[u].nxt[i]=pool[pool[u].fail].nxt[i];
                    }
                }
                order_=order;
                //fail 树上最近模式结尾（BFS 序：fail 深度更浅，先被处理）
                nearest_out_.assign(pool.size(),-1);
                for(int u:order){
                    if(pool[u].out!=-1) nearest_out_[u]=u;
                    else if(u!=0) nearest_out_[u]=nearest_out_[pool[u].fail];
                }
            }
            //文本匹配：返回每个模式串（按 insert 顺序）的出现次数
            std::vector<int> match(const std::string& text) const{
                int m=(int)pat_node.size();
                std::vector<int> cnt(m,0);
                if(m==0) return cnt;
                std::vector<int> freq(pool.size(),0);
                int u=0;
                for(char c:text){
                    u=pool[u].nxt[idx(c)];
                    freq[u]++;
                }
                //fail 树拓扑（BFS 序逆序）累加
                for(int i=(int)order_.size()-1;i>=0;i--){
                    int v=order_[i];
                    freq[pool[v].fail]+=freq[v];
                }
                for(int id=0;id<m;id++) cnt[id]=freq[pat_node[id]];
                return cnt;
            }
            //文本中被任意模式串覆盖的字符总数（各模式覆盖区间取并集；O(len)）
            int covered(const std::string& text) const{
                if(pat_node.empty()) return 0;
                std::vector<int> diff(text.size()+1,0);
                int u=0;
                for(int i=0;i<(int)text.size();i++){
                    u=pool[u].nxt[idx(text[i])];
                    int no=nearest_out_[u];
                    if(no!=-1){
                        int l=i-pat_len_[pool[no].out]+1;
                        if(l<0) l=0;
                        diff[l]++;diff[i+1]--;
                    }
                }
                int cur=0,ans=0;
                for(int i=0;i<(int)text.size();i++){ cur+=diff[i]; if(cur>0) ans++; }
                return ans;
            }
            //每个位置结束的最长匹配模式长度（无匹配 -1；O(len)）
            std::vector<int> longest_match(const std::string& text) const{
                std::vector<int> res(text.size(),-1);
                int u=0;
                for(int i=0;i<(int)text.size();i++){
                    u=pool[u].nxt[idx(text[i])];
                    int no=nearest_out_[u];
                    if(no!=-1) res[i]=pat_len_[pool[no].out];
                }
                return res;
            }
        private:
            std::vector<int> order_;//build 的 BFS 序
            std::vector<int> nearest_out_;//fail 树上最近的模式结尾节点（-1 无）
    };
}
