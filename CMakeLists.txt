cmake_minimum_required(VERSION 3.10)
project(AStarCourseDesign LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

include_directories(include)
set(CORE_SRC src/PathFinder.cpp src/Terrain.cpp src/Visualizer.cpp)

add_executable(astar_demo src/main.cpp ${CORE_SRC})
add_executable(astar_test tests/test.cpp ${CORE_SRC})

enable_testing()
add_test(NAME correctness COMMAND astar_test)
