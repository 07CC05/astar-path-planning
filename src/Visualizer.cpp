#include "Visualizer.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

namespace dsc {

void writeSvg(const std::string& path, const Grid& grid, int cols, int rows,
              Point start, Point goal, const SearchResult& r) {
    const int CELL = 12, TOP = 26;
    std::ostringstream s;
    s << "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"" << cols * CELL
      << "\" height=\"" << rows * CELL + TOP << "\">\n";
    s << "<rect width=\"100%\" height=\"100%\" fill=\"white\"/>\n";
    s << "<text x=\"4\" y=\"17\" font-size=\"13\" font-family=\"sans-serif\">"
      << "expanded=" << r.expanded << " steps=" << r.steps
      << " cost=" << r.cost << "</text>\n";
    s << "<g transform=\"translate(0," << TOP << ")\">\n";
    // 墙
    for (int y = 0; y < rows; ++y)
        for (int x = 0; x < cols; ++x)
            if (grid[y * cols + x] == 1)
                s << "<rect x=\"" << x * CELL << "\" y=\"" << y * CELL
                  << "\" width=\"" << CELL << "\" height=\"" << CELL << "\" fill=\"#334155\"/>\n";
    // 扩展节点（closed）
    for (int i : r.order) {
        int x = i % cols, y = i / cols;
        s << "<rect x=\"" << x * CELL + 1 << "\" y=\"" << y * CELL + 1
          << "\" width=\"" << CELL - 2 << "\" height=\"" << CELL - 2 << "\" fill=\"#dbeafe\"/>\n";
    }
    // 最终路径
    if (r.path.size() > 1) {
        s << "<polyline points=\"";
        for (size_t i = 0; i < r.path.size(); ++i)
            s << (i ? " " : "") << r.path[i].x * CELL + CELL / 2 << "," << r.path[i].y * CELL + CELL / 2;
        s << "\" fill=\"none\" stroke=\"#f59e0b\" stroke-width=\"3\"/>\n";
    }
    s << "<circle cx=\"" << start.x * CELL + CELL / 2 << "\" cy=\"" << start.y * CELL + CELL / 2
      << "\" r=\"5\" fill=\"#16a34a\"/>\n";
    s << "<circle cx=\"" << goal.x * CELL + CELL / 2 << "\" cy=\"" << goal.y * CELL + CELL / 2
      << "\" r=\"5\" fill=\"#2563eb\"/>\n";
    s << "</g>\n</svg>\n";
    std::ofstream out(path);
    out << s.str();
}

void printAscii(const Grid& grid, int cols, int rows,
                Point start, Point goal, const SearchResult& r) {
    std::vector<char> cell(static_cast<size_t>(cols) * rows, '.');
    for (size_t i = 0; i < cell.size(); ++i) if (grid[i] == 1) cell[i] = '#';
    for (int i : r.order) cell[i] = (cell[i] == '#') ? '#' : ':';
    for (const auto& p : r.path) cell[p.y * cols + p.x] = '*';
    cell[start.y * cols + start.x] = 'S';
    cell[goal.y * cols + goal.x] = 'G';
    for (int y = 0; y < rows; ++y) {
        for (int x = 0; x < cols; ++x) std::cout << cell[y * cols + x];
        std::cout << '\n';
    }
}

} // namespace dsc
