// 正确性测试：覆盖堆性质、路径连通性、四算法最优性一致、A* 扩展<=Dijkstra、加权权衡、不可达
#include <algorithm>
#include <iostream>
#include "MinHeap.h"
#include "PathFinder.h"
#include "Terrain.h"

using namespace dsc;

static int pass = 0, fail = 0;
static void ok(bool c, const std::string& msg) {
    if (c) ++pass;
    else { ++fail; std::cout << "  FAIL: " << msg << "\n"; }
}

int main() {
    // 1. 二叉堆弹出升序
    {
        BinaryHeap h;
        std::vector<int> a;
        for (int i = 0; i < 500; ++i) { int v = (i * 37) % 500; a.push_back(v); h.push({double(v), double(v), i}); }
        std::sort(a.begin(), a.end());
        std::vector<int> out;
        while (h.size()) out.push_back(int(h.pop().f));
        ok(out == a, "MinHeap 弹出序列升序");
    }

    // 2. 60 组随机地图：连通性 + 最优性一致 + A* 扩展<=Dijkstra
    const int cols = 31, rows = 21;
    for (uint32_t seed = 1; seed <= 60; ++seed) {
        const char* terr = (seed % 3 == 0) ? "maze" : (seed % 3 == 1 ? "random" : "open");
        Grid g = genGrid(cols, rows, terr, seed);
        Point s{1, 1}, goal{cols - 2, rows - 2};
        clearAround(g, cols, rows, s.x, s.y, 1);
        clearAround(g, cols, rows, goal.x, goal.y, 1);

        struct R { const char* name; Algo a; Heuristic h; SearchResult r; };
        std::vector<R> rs = {
            {"astar", Algo::AStar, Heuristic::Manhattan, {}},
            {"dijkstra", Algo::Dijkstra, Heuristic::None, {}},
            {"bfs", Algo::BFS, Heuristic::None, {}},
            {"greedy", Algo::Greedy, Heuristic::Manhattan, {}},
        };
        for (auto& x : rs) { Options o; o.algo = x.a; o.heuristic = x.h; x.r = search(g, cols, rows, s, goal, o); }

        for (auto& x : rs) {
            if (!x.r.found) continue;
            ok(x.r.path.front().x == s.x && x.r.path.front().y == s.y, std::to_string(seed) + "/" + x.name + " 起点");
            ok(x.r.path.back().x == goal.x && x.r.path.back().y == goal.y, std::to_string(seed) + "/" + x.name + " 终点");
            bool conn = true;
            for (size_t i = 1; i < x.r.path.size(); ++i) {
                if (std::abs(x.r.path[i].x - x.r.path[i-1].x) + std::abs(x.r.path[i].y - x.r.path[i-1].y) != 1) conn = false;
                if (g[x.r.path[i].y * cols + x.r.path[i].x] == 1) conn = false;
            }
            ok(conn, std::to_string(seed) + "/" + x.name + " 四邻接连通且不穿墙");
            ok(x.r.steps == int(x.r.path.size()) - 1, std::to_string(seed) + "/" + x.name + " steps 一致");
        }
        auto find = [&](const char* n) -> SearchResult* { for (auto& x : rs) if (std::string(x.name) == n) return &x.r; return nullptr; };
        auto* a = find("astar"); auto* d = find("dijkstra"); auto* b = find("bfs");
        if (a->found && d->found && b->found)
            ok(a->steps == d->steps && d->steps == b->steps, std::to_string(seed) + " A*/Dijkstra/BFS 最优步数一致");
        if (a->found && d->found)
            ok(a->expanded <= d->expanded, std::to_string(seed) + " A* 扩展数 <= Dijkstra");
    }

    // 3. 加权 A*：扩展更少、路径可能变长
    {
        Grid g = genGrid(41, 31, "random", 7);
        Point s{1, 1}, goal{39, 29};
        clearAround(g, 41, 31, s.x, s.y, 1);
        clearAround(g, 41, 31, goal.x, goal.y, 1);
        Options o1; o1.algo = Algo::AStar; o1.heuristic = Heuristic::Manhattan;
        Options o2 = o1; o2.heuristic = Heuristic::Weighted; o2.weight = 2.0;
        auto r1 = search(g, 41, 31, s, goal, o1);
        auto r2 = search(g, 41, 31, s, goal, o2);
        if (r1.found && r2.found) {
            ok(r2.expanded <= r1.expanded, "加权A* 扩展数 <= 普通A*");
            ok(r2.steps >= r1.steps, "加权A* 步数 >= 普通A*");
        }
    }

    // 4. 不可达：整行墙阻断
    {
        Grid g(10 * 10, 0);
        for (int x = 0; x < 10; ++x) g[5 * 10 + x] = 1;
        Options o; o.algo = Algo::AStar; o.heuristic = Heuristic::Manhattan;
        auto r = search(g, 10, 10, {1,1}, {8,8}, o);
        ok(!r.found, "被墙完全阻断时判不可达");
    }

    std::cout << "\n通过: " << pass << "  失败: " << fail << "\n";
    return fail ? 1 : 0;
}
