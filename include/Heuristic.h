#pragma once
#include <cmath>
#include "types.h"

namespace dsc {

// 启发函数族：曼哈顿/欧氏在 4 邻接下可采纳且一致，保证 A* 最优；
// 加权启发不可采纳，牺牲最优性换取更少扩展。
inline double heuristic(Heuristic kind, int x, int y, int gx, int gy, double w) {
    int dx = std::abs(x - gx), dy = std::abs(y - gy);
    switch (kind) {
        case Heuristic::Manhattan: return double(dx + dy);
        case Heuristic::Euclidean: return std::sqrt(double(dx) * dx + double(dy) * dy);
        case Heuristic::Weighted:  return w * (dx + dy);
        case Heuristic::None:
        default: return 0.0;   // Dijkstra / BFS
    }
}

} // namespace dsc
