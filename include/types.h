#pragma once
#include <vector>
#include <cstdint>

namespace dsc {

struct Point { int x; int y; };

// 网格图：一维数组稠密存储（邻接矩阵的紧凑形式），0=通路，1=障碍
using Grid = std::vector<uint8_t>;

enum class Algo { AStar, Dijkstra, BFS, Greedy };
enum class Heuristic { Manhattan, Euclidean, Weighted, None };
enum class OpenListKind { Heap, Linear };

struct Options {
    Algo algo = Algo::AStar;
    Heuristic heuristic = Heuristic::Manhattan;
    double weight = 2.0;          // 加权 A* 的倍率
    bool diag = false;            // 是否允许 8 邻接
    OpenListKind openList = OpenListKind::Heap;  // 对照实验：堆 / 线性扫描
};

struct SearchResult {
    bool found = false;
    std::vector<Point> path;      // 起点 -> 终点
    std::vector<int> order;       // 扩展顺序（顶点编号），供可视化回放
    long long expanded = 0;       // 实际扩展节点数
    double cost = 0.0;            // 路径总代价
    int steps = -1;               // 路径步数
    double ms = 0.0;              // 计算耗时
};

inline int idx(int x, int y, int cols) { return y * cols + x; }

inline bool blocked(const Grid& g, int x, int y, int cols, int rows) {
    if (x < 0 || y < 0 || x >= cols || y >= rows) return true;
    return g[y * cols + x] == 1;
}

} // namespace dsc
