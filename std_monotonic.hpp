#pragma once
//===== better_std 单调栈 / 单调队列 =====
#include<vector>
#include<deque>
#include<algorithm>
namespace better_std{

// 最大矩形面积（直方图），heights 长度 n，O(n)
long long largest_rectangle(const std::vector<long long>& h){
    int n = (int)h.size();
    std::vector<int> left(n), right(n);
    std::vector<int> st;
    for(int i = 0; i < n; i++){
        while(!st.empty() && h[st.back()] >= h[i]) st.pop_back();
        left[i] = st.empty() ? 0 : st.back() + 1;
        st.push_back(i);
    }
    st.clear();
    for(int i = n - 1; i >= 0; i--){
        while(!st.empty() && h[st.back()] >= h[i]) st.pop_back();
        right[i] = st.empty() ? n - 1 : st.back() - 1;
        st.push_back(i);
    }
    long long ans = 0;
    for(int i = 0; i < n; i++)
        ans = std::max(ans, h[i] * (right[i] - left[i] + 1));
    return ans;
}

// 接雨水：给定高度数组，能接的雨水量（左右扫描）
long long trapping_rain_water(const std::vector<long long>& h){
    int n = (int)h.size();
    if(n == 0) return 0;
    std::vector<long long> L(n), R(n);
    L[0] = h[0];
    for(int i = 1; i < n; i++) L[i] = std::max(L[i-1], h[i]);
    R[n-1] = h[n-1];
    for(int i = n-2; i >= 0; i--) R[i] = std::max(R[i+1], h[i]);
    long long ans = 0;
    for(int i = 0; i < n; i++) ans += std::max(0LL, std::min(L[i], R[i]) - h[i]);
    return ans;
}

// 滑动窗口最大值（单调队列），窗口大小 k（k>=1）
std::vector<long long> mono_queue_max(const std::vector<long long>& a, int k){
    int n = (int)a.size();
    std::vector<long long> res;
    std::deque<int> dq;
    for(int i = 0; i < n; i++){
        while(!dq.empty() && dq.front() <= i - k) dq.pop_front();
        while(!dq.empty() && a[dq.back()] <= a[i]) dq.pop_back();
        dq.push_back(i);
        if(i >= k - 1) res.push_back(a[dq.front()]);
    }
    return res;
}

// 滑动窗口最小值（单调队列），窗口大小 k（k>=1）
std::vector<long long> mono_queue_min(const std::vector<long long>& a, int k){
    int n = (int)a.size();
    std::vector<long long> res;
    std::deque<int> dq;
    for(int i = 0; i < n; i++){
        while(!dq.empty() && dq.front() <= i - k) dq.pop_front();
        while(!dq.empty() && a[dq.back()] >= a[i]) dq.pop_back();
        dq.push_back(i);
        if(i >= k - 1) res.push_back(a[dq.front()]);
    }
    return res;
}

} // namespace better_std
