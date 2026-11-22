#pragma once
#include <array>
#include "types.h"

namespace dsc {

// 邻接关系（图的边）：4 邻接用于三算法公平对比；8 邻接需额外防"穿墙角"
inline const std::array<std::array<int, 2>, 4> DIR4 = {{
    {{1, 0}}, {{-1, 0}}, {{0, 1}}, {{0, -1}}
}};
inline const std::array<std::array<int, 2>, 8> DIR8 = {{
    {{1, 0}}, {{-1, 0}}, {{0, 1}}, {{0, -1}},
    {{1, 1}}, {{1, -1}}, {{-1, 1}}, {{-1, -1}}
}};

} // namespace dsc
