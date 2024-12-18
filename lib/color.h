#ifndef MC_LIB_COLOR_H
#define MC_LIB_COLOR_H

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "block.h"

// clang-format off
#define MC_COLOR_AIR   0.00f, 0.00f, 0.00f, 0.00f
#define MC_COLOR_GRASS 0.25f, 0.75f, 0.20f, 1.00f

/**
 * mcaro - MC_COLOR(X)
 * X: The block type name.
 * Def: The color of the block type.
*/
#define MC_COLOR(X) MC_COLOR_##X
// clang-format on

glm::vec4 getBlockColor(const BlockType blockType);

#endif // MC_LIB_COLOR_H
