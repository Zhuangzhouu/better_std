#pragma once
#include<vector>
#include<string>
namespace better_std{
    //字典树（前缀树）：字符集 [offset, offset+chars)，默认 26 个小写字母
    //数组池实现（根为 0），insert/search/前缀计数/删除
    template<int chars=26,char offset='a'>
    class std_trie{
        private:
            struct Node{
                int nxt[chars];
                int end=0,pre=0;//以该节点结尾的单词数 / 经过该节点的前缀数
                Node(){ for(int i=0;i<chars;i++) nxt[i]=-1; }
            };
            std::vector<Node> pool;
            int words_=0;//不同单词个数
            int idx(char c) const{ return (int)c-offset; }
        public:
            std_trie(){ pool.emplace_back(); }
            void insert(const std::string& s){
                int u=0;pool[u].pre++;
                for(char c:s){
                    int i=idx(c);
                    if(i<0||i>=chars) continue;//非法字符跳过
                    if(pool[u].nxt[i]==-1){
                        pool[u].nxt[i]=(int)pool.size();
                        pool.emplace_back();
                    }
                    u=pool[u].nxt[i];
                    pool[u].pre++;
                }
                pool[u].end++;
                if(pool[u].end==1) words_++;
            }
            //完整单词出现次数
            int count_word(const std::string& s) const{
                int u=0;
                for(char c:s){
                    int i=idx(c);
                    if(i<0||i>=chars) return 0;
                    if(pool[u].nxt[i]==-1) return 0;
                    u=pool[u].nxt[i];
                }
                return pool[u].end;
            }
            bool search(const std::string& s) const{ return count_word(s)>0; }
            //以 s 为前缀的单词数
            int count_prefix(const std::string& s) const{
                int u=0;
                for(char c:s){
                    int i=idx(c);
                    if(i<0||i>=chars) return 0;
                    if(pool[u].nxt[i]==-1) return 0;
                    u=pool[u].nxt[i];
                }
                return pool[u].pre;
            }
            bool starts_with(const std::string& s) const{ return count_prefix(s)>0; }
            //删除一个单词（计数-1，不物理回收节点）；存在才删除
            bool erase(const std::string& s){
                if(count_word(s)==0) return false;
                int u=0;pool[u].pre--;
                for(char c:s){
                    int i=idx(c);
                    u=pool[u].nxt[i];
                    pool[u].pre--;
                }
                pool[u].end--;
                if(pool[u].end==0) words_--;
                return true;
            }
            int size() const{ return words_; }//不同单词数
            int nodes() const{ return (int)pool.size(); }//节点数（含根）
            void clear(){ pool.clear(); pool.emplace_back(); words_=0; }
            //收集以 p 为前缀的所有完整单词（每个不同单词一次，含 p 自身）
            std::vector<std::string> words_with_prefix(const std::string& p) const{
                std::vector<std::string> out;
                int u=0;
                for(char c:p){
                    int i=idx(c);
                    if(i<0||i>=chars||pool[u].nxt[i]==-1) return out;
                    u=pool[u].nxt[i];
                }
                std::string cur=p;
                if(pool[u].end>0) out.push_back(cur);
                collect(u,cur,out);
                return out;
            }
        private:
            void collect(int u,std::string& cur,std::vector<std::string>& out) const{
                for(int i=0;i<chars;i++){
                    int v=pool[u].nxt[i];
                    if(v==-1) continue;
                    cur.push_back(char(offset+i));
                    if(pool[v].end>0) out.push_back(cur);
                    collect(v,cur,out);
                    cur.pop_back();
                }
            }
    };
}
