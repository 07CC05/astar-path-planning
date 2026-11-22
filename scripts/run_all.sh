#!/usr/bin/env bash
# 一键：编译 → 测试 → 四算法对比 → 规模实验 → 动态障碍演示
set -e
cd "$(dirname "$0")/.."
mkdir -p output

echo "[1/5] 编译..."
g++ -std=c++17 -O2 -Wall -Wextra -Iinclude \
    src/main.cpp src/PathFinder.cpp src/Terrain.cpp src/Visualizer.cpp -o astar_demo
g++ -std=c++17 -O2 -Wall -Wextra -Iinclude \
    tests/test.cpp src/PathFinder.cpp src/Terrain.cpp src/Visualizer.cpp -o astar_test

echo "[2/5] 正确性测试..."
./astar_test

echo "[3/5] 四算法对比..."
./astar_demo compare maze 41 27 42

echo "[4/5] 规模实验..."
./astar_demo scale

echo "[5/5] 动态障碍演示..."
./astar_demo dynamic

echo "生成演示 SVG..."
./astar_demo demo astar manhattan maze 41 27 42 > /dev/null

echo "完成。产物见 output/"
