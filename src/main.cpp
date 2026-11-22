// A* 路径规划可视化 —— 数据结构课程设计演示程序
// 用法见 README.md。子命令：demo / compare / scale / dynamic
// 编码规范（指导书4.3）：每行≤60字符，函数体≤40行。
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
#include "StudentInfo.h"
#include "types.h"
#include "PathFinder.h"
#include "Terrain.h"
#include "Visualizer.h"

using namespace dsc;

// 在界面突出显示设计者身份（指导书4.4-1）
static void printIdentity() {
    const std::string line(38, '=');
    std::cout << line << "\n";
    std::cout << " 数据结构课程设计 · A* 路径规划可视化\n";
    std::cout << " 学院: " << STU_COLLEGE << "\n";
    std::cout << " 班级: " << STU_CLASS << "\n";
    std::cout << " 学号: " << STU_ID << "\n";
    std::cout << " 姓名: " << STU_NAME << "\n";
    std::cout << " 指导教师: " << STU_TEACHER << "\n";
    std::cout << line << "\n\n";
}

static void usage(const char* prog) {
    std::cout << "A* Pathfinding - Data Structure Design\n";
    std::cout << "Usage:\n";
    std::cout << "  " << prog
              << " demo [algo] [heur] [terrain]"
                 " [cols] [rows] [seed]\n";
    std::cout << "  " << prog << " compare [terrain]"
              << " [cols] [rows] [seed]\n";
    std::cout << "  " << prog << " scale\n";
    std::cout << "  " << prog << " dynamic\n";
    std::cout << "algo: astar|dijkstra|bfs|greedy\n";
    std::cout << "heur: manhattan|euclidean|"
                 "weighted|none\n";
    std::cout << "terrain: open|random|maze\n";
}

static Algo parseAlgo(const std::string& s) {
    if (s == "dijkstra") return Algo::Dijkstra;
    if (s == "bfs") return Algo::BFS;
    if (s == "greedy") return Algo::Greedy;
    return Algo::AStar;
}

static Heuristic parseHeur(const std::string& s) {
    if (s == "euclidean") return Heuristic::Euclidean;
    if (s == "weighted") return Heuristic::Weighted;
    if (s == "none") return Heuristic::None;
    return Heuristic::Manhattan;
}

static const char* algoName(Algo a) {
    switch (a) {
        case Algo::AStar: return "A*";
        case Algo::Dijkstra: return "Dijkstra";
        case Algo::BFS: return "BFS";
        case Algo::Greedy: return "Greedy";
    }
    return "?";
}

// 解析地形参数：非法值回退到 open（含错误输入处理）
static const char* safeTerrain(const std::string& s) {
    if (s == "random" || s == "maze") return s.c_str();
    return "open";
}

static void cmdDemo(int argc, char** argv) {
    std::string algo = argc > 2 ? argv[2] : "astar";
    std::string heur = argc > 3 ? argv[3] : "manhattan";
    std::string terr = argc > 4 ? argv[4] : "maze";
    int cols = argc > 5 ? std::stoi(argv[5]) : 41;
    int rows = argc > 6 ? std::stoi(argv[6]) : 27;
    uint32_t seed = 42;
    if (argc > 7) {
        seed = static_cast<uint32_t>(std::stoul(argv[7]));
    }

    Grid g = genGrid(cols, rows, safeTerrain(terr), seed);
    Point s{1, 1}, goal{cols - 2, rows - 2};
    clearAround(g, cols, rows, s.x, s.y, 1);
    clearAround(g, cols, rows, goal.x, goal.y, 1);
    Options opt;
    opt.algo = parseAlgo(algo);
    opt.heuristic = parseHeur(heur);
    SearchResult r = search(g, cols, rows, s, goal, opt);

    std::cout << "algo=" << algoName(opt.algo)
              << "  heuristic=" << heur << "\n";
    std::cout << "terrain=" << terr << "  " << cols << "x"
              << rows << "  seed=" << seed << "\n";
    if (!r.found) {
        std::cout << "NOT FOUND (目标不可达)\n";
        return;
    }
    std::cout << "found=true  steps=" << r.steps
              << "  cost=" << r.cost << "\n";
    std::cout << "expanded=" << r.expanded
              << "  ms=" << r.ms << "\n\n";
    printAscii(g, cols, rows, s, goal, r);
    std::string out = "output/demo_" + algo + ".svg";
    writeSvg(out, g, cols, rows, s, goal, r);
    std::cout << "\nSVG written to " << out << "\n";
}

// 四算法对比表中的一行
struct CmpRow {
    const char* name;
    Heuristic h;
    SearchResult r;
};

static void printCmpTable(const std::vector<CmpRow>& rows,
                          int best) {
    std::cout << std::left << std::setw(12) << "Algorithm"
              << std::setw(12) << "Heuristic"
              << std::setw(9) << "Found"
              << std::setw(8) << "Steps"
              << std::setw(10) << "Expanded"
              << std::setw(10) << "ms" << "Optimal\n";
    std::cout << std::string(72, '-') << "\n";
    for (const auto& r : rows) {
        const char* hn = (r.h == Heuristic::None)
                         ? "none" : "manhattan";
        const char* found = r.r.found ? "yes" : "no";
        const char* optimal =
            (r.r.found && r.r.steps == best) ? "YES" : "no";
        std::cout << std::left << std::setw(12) << r.name
                  << std::setw(12) << hn
                  << std::setw(9) << found
                  << std::setw(8) << r.r.steps
                  << std::setw(10) << r.r.expanded
                  << std::setw(10) << std::fixed
                  << std::setprecision(3) << r.r.ms
                  << optimal << "\n";
    }
}

static void cmdCompare(int argc, char** argv) {
    std::string terr = argc > 2 ? argv[2] : "maze";
    int cols = argc > 3 ? std::stoi(argv[3]) : 41;
    int rows = argc > 4 ? std::stoi(argv[4]) : 27;
    uint32_t seed = 42;
    if (argc > 5) {
        seed = static_cast<uint32_t>(std::stoul(argv[5]));
    }
    Grid g = genGrid(cols, rows, safeTerrain(terr), seed);
    Point s{1, 1}, goal{cols - 2, rows - 2};
    clearAround(g, cols, rows, s.x, s.y, 1);
    clearAround(g, cols, rows, goal.x, goal.y, 1);

    const std::vector<std::pair<Algo, Heuristic>> combos = {
        {Algo::AStar, Heuristic::Manhattan},
        {Algo::Dijkstra, Heuristic::None},
        {Algo::BFS, Heuristic::None},
        {Algo::Greedy, Heuristic::Manhattan},
    };
    std::vector<CmpRow> out;
    for (const auto& c : combos) {
        Options o;
        o.algo = c.first;
        o.heuristic = c.second;
        SearchResult r = search(g, cols, rows, s, goal, o);
        out.push_back({algoName(c.first), c.second, r});
    }
    int best = 1 << 30;
    for (const auto& x : out)
        if (x.r.found) best = std::min(best, x.r.steps);

    std::cout << "terrain=" << terr << "  "
              << cols << "x" << rows << "  seed=" << seed
              << "  (同一地图、同一起终点)\n\n";
    printCmpTable(out, best);
}

// 单个规模的堆/线性对照实验
static void scaleOne(int n) {
    Grid g = genGrid(n, n, "open", 7);
    Point s{1, 1}, goal{n - 2, n - 2};
    clearAround(g, n, n, s.x, s.y, 1);
    clearAround(g, n, n, goal.x, goal.y, 1);
    Options oh;
    oh.algo = Algo::AStar;
    oh.heuristic = Heuristic::Manhattan;
    oh.openList = OpenListKind::Heap;
    Options ol = oh;
    ol.openList = OpenListKind::Linear;
    SearchResult rh = search(g, n, n, s, goal, oh);
    SearchResult rl = search(g, n, n, s, goal, ol);
    double ratio = (rh.ms > 0) ? rl.ms / rh.ms : 0;
    std::string label = std::to_string(n) + "x"
                        + std::to_string(n);
    std::string rs = std::to_string(ratio).substr(0, 4);
    rs += "x";
    std::cout << std::left << std::setw(10) << label
              << std::setw(14) << std::fixed
              << std::setprecision(3) << rh.ms
              << std::setw(16) << rl.ms
              << rs << "\n";
}

static void cmdScale() {
    std::cout << "Open-list 规模实验：二叉堆 vs 线性扫描\n";
    std::cout << "（open 地形，稀疏障碍，A*+曼哈顿）\n\n";
    std::cout << std::left << std::setw(10) << "Grid"
              << std::setw(14) << "Heap(ms)"
              << std::setw(16) << "Linear(ms)"
              << std::setw(10) << "Ratio\n";
    std::cout << std::string(50, '-') << "\n";
    const int sizes[] = {64, 128, 256, 512};
    for (int n : sizes) scaleOne(n);
}

static void cmdDynamic() {
    int cols = 41, rows = 27;
    Grid g = genGrid(cols, rows, "open", 3);
    Point s{1, 1}, goal{cols - 2, rows - 2};
    clearAround(g, cols, rows, s.x, s.y, 1);
    clearAround(g, cols, rows, goal.x, goal.y, 1);
    Options o;
    o.algo = Algo::AStar;
    o.heuristic = Heuristic::Manhattan;
    SearchResult before = search(g, cols, rows, s, goal, o);
    if (!before.found) {
        std::cout << "初始路径不可达\n";
        return;
    }

    // 在路径中段新增连续 4 格路障，强制绕行以体现重规划开销
    Point block = before.path[before.path.size() / 2];
    int mid = static_cast<int>(before.path.size()) / 2;
    for (int i = mid - 1; i <= mid + 2 &&
            i < static_cast<int>(before.path.size()); ++i) {
        if (i <= 0) continue;
        g[before.path[i].y * cols + before.path[i].x] = 1;
    }
    SearchResult after = search(g, cols, rows, s, goal, o);

    std::cout << "动态障碍重规划演示（在路径中段 ("
              << block.x << "," << block.y
              << ") 附近新增 4 格路障）\n\n";
    std::cout << "重规划前: steps=" << before.steps
              << "  expanded=" << before.expanded << "\n";
    if (after.found) {
        int d = after.expanded - before.expanded;
        std::cout << "重规划后: steps=" << after.steps
                  << "  expanded=" << after.expanded
                  << "  (扩展变化 " << (d >= 0 ? "+" : "") << d
                  << ")\n";
    } else {
        std::cout << "重规划后: 无可行路径\n";
    }
    std::cout << "说明：完整重规划清空 cameFrom/closed 后从起点重搜；\n"
              << "      启发仍可采纳则结果依旧最优。扩展数可能增减，\n"
              << "      若要复用旧信息需增量式 LPA*/D* Lite（超范围）。\n";
    const char* svg = "output/dynamic_after.svg";
    writeSvg(svg, g, cols, rows, s, goal, after);
}

int main(int argc, char** argv) {
    printIdentity();
    if (argc < 2) {
        usage(argv[0]);
        return 0;
    }
    std::string cmd = argv[1];
    if (cmd == "demo") cmdDemo(argc, argv);
    else if (cmd == "compare") cmdCompare(argc, argv);
    else if (cmd == "scale") cmdScale();
    else if (cmd == "dynamic") cmdDynamic();
    else usage(argv[0]);
    return 0;
}
