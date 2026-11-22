#include "Terrain.h"
#include <cstring>
#include <string>
#include <vector>
#include <deque>
#include <algorithm>

namespace dsc {

uint32_t lcg(uint32_t& seed) {
    seed = seed * 1664525u + 1013904223u;
    return seed;
}

// 递归分割：把矩形区域用一道带缺口的墙一分为二，再递归两侧，天然保证连通
static void divide(Grid& g, uint32_t& seed, int x0, int y0, int x1, int y1, int cols) {
    int w = x1 - x0 + 1, h = y1 - y0 + 1;
    // 区域过小则不再分割，避免退化出封死的死房间
    if (w < 5 || h < 5) return;
    bool horizontal;
    if (w > h) horizontal = false;
    else if (h > w) horizontal = true;
    else horizontal = (lcg(seed) & 1) != 0;

    if (horizontal) {
        // 墙线落在内部奇数行；缺口限制在 [x0+1, x1-1]，绝不在端点
        int wy = y0 + 1 + 2 * static_cast<int>(lcg(seed) % static_cast<uint32_t>((h - 2) / 2));
        int gap = x0 + 1 + static_cast<int>(lcg(seed) % static_cast<uint32_t>(w - 2));
        for (int x = x0; x <= x1; ++x) if (x != gap) g[wy * cols + x] = 1;
        divide(g, seed, x0, y0, x1, wy - 1, cols);
        divide(g, seed, x0, wy + 1, x1, y1, cols);
    } else {
        int wx = x0 + 1 + 2 * static_cast<int>(lcg(seed) % static_cast<uint32_t>((w - 2) / 2));
        int gap = y0 + 1 + static_cast<int>(lcg(seed) % static_cast<uint32_t>(h - 2));
        for (int y = y0; y <= y1; ++y) if (y != gap) g[y * cols + wx] = 1;
        divide(g, seed, x0, y0, wx - 1, y1, cols);
        divide(g, seed, wx + 1, y0, x1, y1, cols);
    }
}

// 连通性修复：递归分割可能因缺口落在端点而封出死房间。
// 从目标点向"已连通区"反向凿一条最短墙通道，保证起点可达终点（工程兜底）。
static void repairConnectivity(Grid& g, int cols, int rows, Point from, Point to) {
    const int N = cols * rows;
    std::vector<uint8_t> seen(N, 0);
    std::deque<int> q;
    int s = to.y * cols + to.x;
    seen[s] = 1; q.push_back(s);
    std::vector<int> prev(N, -1);
    int target = -1;
    const int D[4][2] = {{1,0},{-1,0},{0,1},{0,-1}};
    // 在"忽略墙"的图上从终点找一条到起点的路，沿途哪些是墙就凿开
    while (!q.empty()) {
        int cur = q.front(); q.pop_front();
        int cx = cur % cols, cy = cur / cols;
        if (cx == from.x && cy == from.y) { target = cur; break; }
        for (auto& d : D) {
            int nx = cx + d[0], ny = cy + d[1];
            if (nx < 1 || ny < 1 || nx >= cols - 1 || ny >= rows - 1) continue;
            int ni = ny * cols + nx;
            if (seen[ni]) continue;
            seen[ni] = 1; prev[ni] = cur; q.push_back(ni);
        }
    }
    if (target < 0) return;
    for (int p = target; p != -1; p = prev[p]) g[p] = 0; // 凿通整条通道
}

Grid genGrid(int cols, int rows, const char* type, uint32_t seed) {
    Grid g(static_cast<size_t>(cols) * rows, 0);
    std::string t(type);
    if (t == "maze") {
        for (int x = 0; x < cols; ++x) { g[x] = 1; g[(rows - 1) * cols + x] = 1; }
        for (int y = 0; y < rows; ++y) { g[y * cols] = 1; g[y * cols + cols - 1] = 1; }
        divide(g, seed, 1, 1, cols - 2, rows - 2, cols);
        // 兜底：保证约定起终点 (1,1) 与 (cols-2,rows-2) 连通
        repairConnectivity(g, cols, rows, Point{1,1}, Point{cols-2,rows-2});
    } else {
        double p = (t == "random") ? 0.23 : 0.06; // random / open
        for (size_t i = 0; i < g.size(); ++i)
            g[i] = (lcg(seed) / 4294967296.0 < p) ? 1 : 0;
    }
    return g;
}

void clearAround(Grid& g, int cols, int rows, int x, int y, int r) {
    for (int dy = -r; dy <= r; ++dy)
        for (int dx = -r; dx <= r; ++dx) {
            int nx = x + dx, ny = y + dy;
            // 不清除最外圈：迷宫的外墙必须保持完整，否则起点/终点区会被封成孤岛
            if (nx >= 1 && ny >= 1 && nx < cols - 1 && ny < rows - 1)
                g[ny * cols + nx] = 0;
        }
}

} // namespace dsc
