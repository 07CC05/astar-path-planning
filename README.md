# A* 路径规划可视化 — 数据结构课程设计

自主选题项目。以网格图为背景，实现 A*、Dijkstra、BFS、贪心最佳优先四种搜索算法，自制二叉最小堆作为优先队列，完成启发函数对比、动态障碍重规划与 open list 规模实验。C++17 编写，零第三方依赖。

**设计者**：2025 级计科（1）班 王宇　**指导教师**：苏畅

> **在线演示**：https://07cc05.github.io/astar-path-planning/
> 浏览器里可以直接试：点击或拖动画障碍，右键擦除，四种算法的动画对比和动态重规划都能看到，不用装任何东西。

> 本报告与工程严格遵循《数据结构实验与课程设计指导书》第 4 章要求：
> 采用 C++17 编程；程序启动界面突出显示班级、学号、姓名（4.4-1）；
> 报告采用学校统一封面并按"需求分析/概要设计/详细设计/调试分析/
> 用户使用说明/测试结果/附录"七部分组织（4.3-五）；正文宋体小四、
> 首行缩进 2 字符、单倍行距；源码每行不超过 60 字符、函数体不超过
> 40 行（4.3-二）；提交物按"源程序/可执行程序/测试用例/报告文档"
> 分类置于 submit/ 目录（4.4-2）。

## 一、目录结构

```
ds_course_design/
├── CMakeLists.txt          CMake 构建脚本（可选）
├── Makefile                make 构建脚本（可选）
├── README.md               本文件：运行与使用说明
├── include/                头文件（算法的"接口与数据结构"层）
│   ├── types.h             公共类型：Point / Grid / Options / SearchResult
│   ├── MinHeap.h           自制二叉最小堆 + 线性扫描对照 + open list 抽象接口
│   ├── GridGraph.h         网格图邻接方向表（4 邻接 / 8 邻接）
│   ├── Heuristic.h         启发函数：曼哈顿 / 欧氏 / 加权 / 无
│   ├── PathFinder.h        搜索算法声明
│   ├── Terrain.h           地形生成声明
│   ├── Visualizer.h        ASCII / SVG 输出声明
│   └── StudentInfo.h       设计者身份宏（界面与封面共用，按此填写）
├── src/                    实现文件（算法的"实现"层）
│   ├── PathFinder.cpp      统一搜索框架：BFS 队列 + 三种堆式搜索
│   ├── Terrain.cpp         LCG 随机数、递归分割迷宫、连通性兜底
│   ├── Visualizer.cpp      ASCII 终端打印、SVG 文件导出
│   └── main.cpp            命令行入口：demo / compare / scale / dynamic
├── tests/
│   └── test.cpp            正确性测试（1066 项断言）
├── scripts/
│   └── run_all.sh          一键：编译 → 测试 → 实验 → 出图
├── output/                 运行产物（SVG 图、实验结果记录）
├── docs/                   报告与答辩材料
│   ├── 01_报告框架.md
│   ├── 02_知识点映射表.md
│   ├── 04_常见问题.md
│   └── index.html          交互式可视化报告（GitHub Pages 首页）
└── submit/                 课程提交物（源程序 / 可执行程序 / 测试用例 / 报告 / 实验结果）
```

> **说明**：`submit/`、`*.exe` 和报告 WORD 没放进仓库，里面带姓名学号，体积也大；
> `include/StudentInfo.h` 里的学号也打了码。完整提交版在本地。
>
> 关于 `docs/index.html`：这份网页是用 JavaScript 重写的，方便在浏览器里直接点。
> 真正的工程在 `include/` 和 `src/`，是 C++17，两边代码互相独立。C++ 版的运行结果在 `output/` 里，是 SVG 图。

## 二、编译

工程为 C++17、零依赖。任选一种方式：

**方式 A：g++ 直接编译（无需 make / cmake）**
```bash
g++ -std=c++17 -O2 -Wall -Wextra -static -Iinclude \
    src/main.cpp src/PathFinder.cpp src/Terrain.cpp src/Visualizer.cpp -o astar_demo

g++ -std=c++17 -O2 -Wall -Wextra -static -Iinclude \
    tests/test.cpp src/PathFinder.cpp src/Terrain.cpp src/Visualizer.cpp -o astar_test
```
Windows 下把输出名改为 `astar_demo.exe` / `astar_test.exe`。
`-static` 会把 MinGW 运行库（libstdc++-6.dll、libgcc_s_seh-1.dll）
一起打包进 exe，换台电脑直接双击即可运行；不加则那台电脑需有
MinGW 的 bin 目录在 PATH 中，否则会提示缺少 DLL。

**方式 B：CMake**
```bash
cmake -S . -B build && cmake --build build
cd build && ctest --output-on-failure
```

**方式 C：make**（需安装 GNU make）
```bash
make && make test
```

## 三、运行命令

**方式一：双击运行（推荐演示时使用）**

直接双击 `astar_demo.exe`，程序显示身份信息后给出功能菜单，
输入序号回车即可运行对应演示，运行结束按回车返回菜单，输入 `0` 退出。
双击 `astar_test.exe` 会执行全部 1066 项断言并在结束时等待回车。

**方式二：命令行运行**

以下命令在工程根目录执行。

### 1. 单次搜索演示 `demo`
```bash
./astar_demo demo <算法> <启发函数> <地形> [列数] [行数] [随机种子]
```
- 算法：`astar` / `dijkstra` / `bfs` / `greedy`
- 启发函数：`manhattan` / `euclidean` / `weighted` / `none`
- 地形：`open`（稀疏障碍）/ `random`（随机障碍）/ `maze`（递归迷宫，保证连通）
- 示例：
```bash
./astar_demo demo astar manhattan maze 41 27 42
./astar_demo demo dijkstra none random 31 21 7
```
输出：统计信息（步数/代价/扩展节点数/耗时）+ 终端 ASCII 地图（S 起点、G 终点、# 墙、* 路径、: 扩展节点），并生成 `output/demo_<算法>.svg`。

### 2. 四算法横向对比 `compare`
```bash
./astar_demo compare [地形] [列数] [行数] [随机种子]
# 例：./astar_demo compare maze 41 27 42
```
同一地图、同一起终点跑四种算法，输出步数、扩展节点数、耗时与最优性判定。这是报告"参照系"纪律要求的标准动作。

### 3. open list 规模实验 `scale`
```bash
./astar_demo scale
```
在 64²/128²/256²/512² 网格上对比"自制二叉堆"与"线性扫描"两种 open list 的耗时，量化 O(log n) 相对 O(n) 的收益。

### 4. 动态障碍重规划 `dynamic`
```bash
./astar_demo dynamic
```
先在开阔地形求出路径，在路径中点插入突发障碍，清空 cameFrom/closed 后完整重规划，输出前后扩展节点数对比，并生成 `output/dynamic_after.svg`。

### 5. 正确性测试
```bash
./astar_test
```
覆盖 6 类断言：二叉堆弹出升序、路径四邻接连通且不穿墙、四算法最优步数一致、A* 扩展数 ≤ Dijkstra、加权启发权衡、不可达判定。当前 **1066 项全部通过**。

### 6. 一键运行
```bash
bash scripts/run_all.sh
```
依次完成编译、测试、四算法对比、规模实验、动态演示，结果汇总到 `output/实验结果记录.txt`。

## 四、阅读源码的建议顺序

1. `include/types.h` — 先理解数据如何表示（网格=一维数组，顶点=y*cols+x）。
2. `include/MinHeap.h` — 核心数据结构：二叉堆的上滤/下滤，以及线性扫描对照。
3. `include/GridGraph.h` + `include/Heuristic.h` — 图的边（邻接方向）与启发函数。
4. `src/PathFinder.cpp` — 统一搜索框架，重点是 open list 如何替换、优先级如何按算法变化。
5. `src/Terrain.cpp` — 递归分割迷宫与连通性兜底。
6. `src/main.cpp` — 四个演示子命令。

## 五、参数与约定

- 起终点固定为 `(1,1)` 与 `(cols-2, rows-2)`，均在地图内圈。
- 4 邻接每步代价 1；8 邻接对角步代价 √2 且禁止穿墙角（命令行演示默认 4 邻接）。
- 加权启发默认倍率 2.0，不可采纳，因此不保证最优（用于对照）。
- 随机数用 LCG，同一种子生成同一地图，实验可复现。
- 迷宫生成带连通性兜底，保证约定起终点始终可达。

## 六、常见问题

- **编译报错 `make: command not found`**：用上面"方式 A"的 g++ 直接编译即可。
- **SVG 用浏览器打开**：`output/*.svg` 可直接拖入浏览器查看，橙色折线为最终路径。
- **中文乱码**：程序启动时已把控制台代码页切到 UTF-8（65001），正常双击不会乱码；
  若仍显示方块，右键窗口标题栏 → 属性 → 字体改为“新宋体”或“等线”。
  ASCII 地图本身为纯 ASCII 字符，不受编码影响。
- **提示缺少 `libstdc++-6.dll`**：运行的是未加 `-static` 编译的版本；改用
  `-static -static-libgcc -static-libstdc++` 重新编译，或直接使用随包提供的 exe。
