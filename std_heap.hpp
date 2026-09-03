#pragma once
#include<iostream>
#include<vector>
#include<functional>
#include<algorithm>
namespace better_std{
    //二叉堆（默认大根堆，与 std::priority_queue 语义一致）
    //Comp 为"a 优先级低于 b"的判定：默认 std::less<T> → 大根堆
    template<typename T=long long,class Comp=std::less<T>>
    class std_heap{
        private:
            std::vector<T> h_;
            Comp comp_;
            void sift_up(int i){
                while(i>0){
                    int p=(i-1)/2;
                    if(!comp_(h_[p],h_[i])) break;
                    std::swap(h_[i],h_[p]);
                    i=p;
                }
            }
            void sift_down(int i){
                int n=(int)h_.size();
                while(true){
                    int best=i;
                    int l=2*i+1,r=2*i+2;
                    if(l<n&&comp_(h_[best],h_[l])) best=l;
                    if(r<n&&comp_(h_[best],h_[r])) best=r;
                    if(best==i) break;
                    std::swap(h_[i],h_[best]);
                    i=best;
                }
            }
        public:
            std_heap(){}
            std_heap(const std::vector<T>& v):h_(v){ for(int i=(int)h_.size()/2-1;i>=0;i--) sift_down(i); }
            bool empty() const{ return h_.empty(); }
            int size() const{ return (int)h_.size(); }
            void clear(){ h_.clear(); }
            void push(const T& x){
                h_.push_back(x);
                sift_up((int)h_.size()-1);
            }
            const T& top() const{ return h_.front(); }
            void pop(){
                std::swap(h_.front(),h_.back());
                h_.pop_back();
                if(!h_.empty()) sift_down(0);
            }
            //内部数组拷贝
            std::vector<T> to_vector() const{ return h_; }
            //按堆序弹出（默认大根堆→降序），不修改原堆
            std::vector<T> heap_sort() const{
                std_heap<T,Comp> copy=*this;
                std::vector<T> res;
                res.reserve(copy.size());
                while(!copy.empty()){ res.push_back(copy.top()); copy.pop(); }
                return res;
            }
    };
}
