#pragma once
#include "types.h"

namespace dsc {

// 统一图搜索：BFS 用 FIFO 队列；A*/Dijkstra/贪心用 open list（二叉堆或线性扫描）
SearchResult search(const Grid& grid, int cols, int rows,
                    Point start, Point goal, const Options& opt);

} // namespace dsc
