#pragma once
#include <string>
#include "types.h"

namespace dsc {

// 把一次搜索结果导出为 SVG（起点绿、终点蓝、墙深灰、扩展浅蓝、路径橙色折线）
void writeSvg(const std::string& path, const Grid& grid, int cols, int rows,
              Point start, Point goal, const SearchResult& r);

// 在终端用 ASCII 打印地图与路径（S=起点 G=终点 #=墙 *=路径 ·=扩展）
void printAscii(const Grid& grid, int cols, int rows,
                Point start, Point goal, const SearchResult& r);

} // namespace dsc
