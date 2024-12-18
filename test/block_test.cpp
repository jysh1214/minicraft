/**
 * @file block_test.cpp
 * @author Alex Chiang
 * @brief
 */
#include <gtest/gtest.h>

#include "utils.h"

#include "block.h"
#include "color.h"

namespace {

// clang-format off
constexpr GLfloat GrassBlockVertex[] = {
  1.0f, 1.0f, 1.0f, MC_COLOR(GRASS),
  1.0f, 0.0f, 1.0f, MC_COLOR(GRASS),
  1.0f, 1.0f, 0.0f, MC_COLOR(GRASS),
  1.0f, 0.0f, 0.0f, MC_COLOR(GRASS),
  0.0f, 1.0f, 1.0f, MC_COLOR(GRASS),
  0.0f, 1.0f, 0.0f, MC_COLOR(GRASS),
  0.0f, 0.0f, 1.0f, MC_COLOR(GRASS),
  0.0f, 0.0f, 0.0f, MC_COLOR(GRASS)
};
// clang-format on

} // namespace

/**
 * Init block and check the color.
 */
TEST(BlockColorTest, CheckColor) {
  Block<VertexFormat::Color> block(BlockType::Grass);

  // std::cout << "[DEBUG] \n";
  // debug_print(block.vertex.data(), 7, 8);

  // check the color
  bool haveSameColor = std::equal(std::begin(block.vertex),
                                  std::end(block.vertex), GrassBlockVertex);
  EXPECT_TRUE(haveSameColor);
}

/**
 * Init block and translate it.
 */
TEST(BlockTranslateTest, CheckPoisition) {
  Block<VertexFormat::Color> block(BlockType::Grass);
}
