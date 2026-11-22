#pragma once
#include <cstdint>
#include "types.h"

namespace dsc {

// 带种子的线性同余随机数，保证地图可复现（实验可重做）
uint32_t lcg(uint32_t& seed);

// 生成地形：open(稀疏障碍) / random(随机障碍) / maze(递归分割迷宫，保证连通)
Grid genGrid(int cols, int rows, const char* type, uint32_t seed);

// 清空以 (x,y) 为中心半径 r 的区域（保证起点/终点可站立）
void clearAround(Grid& g, int cols, int rows, int x, int y, int r);

} // namespace dsc
