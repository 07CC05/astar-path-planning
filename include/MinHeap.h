#pragma once
#include <vector>
#include <cstddef>
#include <utility>
#include "types.h"

namespace dsc {

// open list 中的条目：f 为优先级，h 为启发值（用于 tie-break，f 相同优先扩展更接近终点者）
struct OpenEntry { double f; double h; int idx; };

inline bool better(const OpenEntry& a, const OpenEntry& b) {
    if (a.f != b.f) return a.f < b.f;
    return a.h < b.h;
}

// open list 抽象接口：便于"二叉堆 vs 线性扫描"对照实验替换实现
class IOpenList {
public:
    virtual ~IOpenList() = default;
    virtual void push(const OpenEntry& e) = 0;
    virtual OpenEntry pop() = 0;
    virtual bool empty() const = 0;
    virtual std::size_t size() const = 0;
};

// 【核心数据结构】自制二叉最小堆
// 数组存储完全二叉树；push 上滤、pop 下滤，均为 O(log n)。
// 采用 lazy-deletion：允许重复入堆，出堆时由 closed 数组判重，
// 从而避免实现复杂的 decrease-key（这是工程上常用的取舍）。
class BinaryHeap final : public IOpenList {
    std::vector<OpenEntry> a_;
public:
    void push(const OpenEntry& e) override {
        a_.push_back(e);
        std::size_t i = a_.size() - 1;
        while (i > 0) {
            std::size_t p = (i - 1) / 2;
            if (better(a_[i], a_[p])) { std::swap(a_[i], a_[p]); i = p; }
            else break;
        }
    }
    OpenEntry pop() override {
        OpenEntry top = a_[0];
        OpenEntry last = a_.back();
        a_.pop_back();
        if (!a_.empty()) {
            a_[0] = last;
            std::size_t i = 0, n = a_.size();
            while (true) {
                std::size_t l = 2 * i + 1, r = l + 1, s = i;
                if (l < n && better(a_[l], a_[s])) s = l;
                if (r < n && better(a_[r], a_[s])) s = r;
                if (s == i) break;
                std::swap(a_[i], a_[s]);
                i = s;
            }
        }
        return top;
    }
    bool empty() const override { return a_.empty(); }
    std::size_t size() const override { return a_.size(); }
};

// 【对照组】线性扫描 open list：vector + 每轮 O(n) 找最小并擦除
// 用于规模实验，量化"用堆而非线性结构"的收益。
class LinearOpenList final : public IOpenList {
    std::vector<OpenEntry> a_;
public:
    void push(const OpenEntry& e) override { a_.push_back(e); }
    OpenEntry pop() override {
        std::size_t best = 0;
        for (std::size_t i = 1; i < a_.size(); ++i)
            if (better(a_[i], a_[best])) best = i;
        OpenEntry top = a_[best];
        a_[best] = a_.back();
        a_.pop_back();
        return top;
    }
    bool empty() const override { return a_.empty(); }
    std::size_t size() const override { return a_.size(); }
};

} // namespace dsc
