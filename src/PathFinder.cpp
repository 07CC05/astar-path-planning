#include "PathFinder.h"
#include "MinHeap.h"
#include "GridGraph.h"
#include "Heuristic.h"
#include <chrono>
#include <deque>
#include <limits>
#include <memory>

namespace dsc {

SearchResult search(const Grid& grid, int cols, int rows,
                    Point start, Point goal, const Options& opt) {
    using Clock = std::chrono::high_resolution_clock;
    const auto t0 = Clock::now();
    const int N = cols * rows;
    const int s = idx(start.x, start.y, cols);
    const int g = idx(goal.x, goal.y, cols);
    SearchResult res;

    if (start.x < 0 || start.y < 0 || start.x >= cols || start.y >= rows ||
        goal.x < 0 || goal.y < 0 || goal.x >= cols || goal.y >= rows ||
        grid[s] == 1 || grid[g] == 1) {
        res.ms = std::chrono::duration<double, std::milli>(Clock::now() - t0).count();
        return res; // 不可达 / 非法输入
    }

    const double INF = std::numeric_limits<double>::infinity();
    std::vector<double> gScore(N, INF);
    std::vector<int> cameFrom(N, -1);
    std::vector<uint8_t> closed(N, 0);
    gScore[s] = 0.0;

    const bool useHeap = (opt.algo != Algo::BFS);
    std::unique_ptr<IOpenList> open;
    if (useHeap)
        open.reset(opt.openList == OpenListKind::Heap ? static_cast<IOpenList*>(new BinaryHeap())
                                                      : static_cast<IOpenList*>(new LinearOpenList()));
    std::deque<int> queue; // BFS 专用 FIFO

    auto priority = [&](int i) -> std::pair<double,double> {
        int x = i % cols, y = i / cols;
        double gg = gScore[i];
        double h = heuristic(opt.heuristic, x, y, goal.x, goal.y, opt.weight);
        if (opt.algo == Algo::Dijkstra || opt.algo == Algo::BFS) return {gg, 0.0};
        if (opt.algo == Algo::Greedy) return {h, h};
        return {gg + h, h}; // A*
    };

    auto sp = priority(s);
    if (useHeap) open->push({sp.first, sp.second, s});
    else queue.push_back(s);

    bool found = false;
    while (useHeap ? !open->empty() : !queue.empty()) {
        int cur;
        if (useHeap) cur = open->pop().idx;
        else { cur = queue.front(); queue.pop_front(); }
        if (closed[cur]) continue;           // lazy-deletion 判重
        closed[cur] = 1;
        res.expanded++;
        res.order.push_back(cur);
        if (cur == g) { found = true; break; }

        int cx = cur % cols, cy = cur / cols;
        for (int k = 0; k < (opt.diag ? 8 : 4); ++k) {
            const auto& d = opt.diag ? DIR8[k] : DIR4[k];
            int nx = cx + d[0], ny = cy + d[1];
            if (blocked(grid, nx, ny, cols, rows)) continue;
            int ni = idx(nx, ny, cols);
            if (closed[ni]) continue;
            bool isDiag = (d[0] != 0 && d[1] != 0);
            if (isDiag && (blocked(grid, nx, cy, cols, rows) || blocked(grid, cx, ny, cols, rows)))
                continue; // 禁止穿墙角
            double step = isDiag ? 1.4142135623730951 : 1.0;
            double tentative = gScore[cur] + step;
            if (tentative < gScore[ni]) {
                gScore[ni] = tentative;
                cameFrom[ni] = cur;
                auto np = priority(ni);
                if (useHeap) open->push({np.first, np.second, ni});
                else queue.push_back(ni);
            }
        }
    }

    if (found) {
        int p = g;
        while (p != -1) { res.path.push_back({p % cols, p / cols}); p = cameFrom[p]; }
        std::vector<Point> rev(res.path.rbegin(), res.path.rend());
        res.path.swap(rev);
        res.found = true;
        res.cost = gScore[g];
        res.steps = static_cast<int>(res.path.size()) - 1;
    }
    res.ms = std::chrono::duration<double, std::milli>(Clock::now() - t0).count();
    return res;
}

} // namespace dsc
