#include "color.h"

namespace {

// clang-format off
constexpr glm::vec4 BlockColor[] = {
  { MC_COLOR(AIR)   },
  { MC_COLOR(GRASS) }
};
// clang-format on

} // namespace

glm::vec4 getBlockColor(const BlockType blockType) {
  return BlockColor[static_cast<GLuint>(blockType)];
}
