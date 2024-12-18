/**
 * @file block_test.cpp
 * @author Alex Chiang
 * @brief
 */
#include <iterator>

#include <gtest/gtest.h>

#include "utils.h"

#include "block.h"
#include "chunk.h"
#include "data.h"
#include "vertex_info.h"

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

template <VertexFormat vertexFormat> struct Wrapper {
  static constexpr VertexFormat format = vertexFormat;
};

template <typename vertexFormat> class ChunkTest : public ::testing::Test {
protected:
  static constexpr VertexFormat format = vertexFormat::format;
  void SetUp() override {}
  void TearDown() override { Chunk<format>::reset(); }
};

template <typename vertexFormat>
class ChunkPanicTest : public ChunkTest<vertexFormat> {
protected:
  static constexpr VertexFormat format = vertexFormat::format;
  void TearDown() override { Chunk<format>::reset(); }
};

using TestVertexType = ::testing::Types<Wrapper<VertexFormat::Color>>;
TYPED_TEST_SUITE(ChunkTest, TestVertexType);
TYPED_TEST_SUITE(ChunkPanicTest, TestVertexType);

} // namespace

TYPED_TEST(ChunkPanicTest, NoSetSize) {
  std::vector<GLuint> blockMap;
  EXPECT_THROW(Chunk<VertexFormat::Color> _(blockMap), std::runtime_error);
}

TYPED_TEST(ChunkPanicTest, SetSizeTwice) {
  using Chunk_T = Chunk<VertexFormat::Color>;
  Chunk_T::setSize({4, 4, 4});
  EXPECT_THROW(Chunk_T::setSize({2, 2, 2}), std::runtime_error);
}

TYPED_TEST(ChunkPanicTest, IllegalSizeVal) {
  std::vector<GLuint> blockMap;
  using Chunk_T = Chunk<VertexFormat::Color>;
  EXPECT_THROW(Chunk_T::setSize({0, 0, 0}), std::runtime_error);
}

TYPED_TEST(ChunkPanicTest, IllegalBlockMapSize) {
  using Chunk_T = Chunk<VertexFormat::Color>;
  Chunk_T::setSize({4, 4, 4});

  std::vector<GLuint> blockMap(10); // != 4*4*4
  EXPECT_THROW(Chunk<VertexFormat::Color> _(blockMap), std::runtime_error);
}

TYPED_TEST(ChunkTest, FillGrassBlock) {
  const GLuint xSize = 4;
  const GLuint ySize = 4;
  const GLuint zSize = 4;

  std::vector<GLuint> blockMap(xSize * ySize * zSize);
  std::fill(std::begin(blockMap), std::end(blockMap),
            static_cast<GLuint>(BlockType::Grass));

  using Chunk_T = Chunk<VertexFormat::Color>;
  Chunk_T::setSize({xSize, ySize, zSize});

  EXPECT_NO_THROW(Chunk<VertexFormat::Color> _(blockMap));
}

TYPED_TEST(ChunkTest, NoInitChunk) {
  const GLuint xSize = 2;
  const GLuint ySize = 2;
  const GLuint zSize = 2;
  const GLuint size = xSize * ySize * zSize;

  std::vector<GLuint> blockMap(xSize * ySize * zSize);
  std::fill(std::begin(blockMap), std::end(blockMap),
            static_cast<GLuint>(BlockType::Grass));

  using Chunk_T = Chunk<VertexFormat::Color>;
  Chunk_T::setSize({xSize, ySize, zSize});

  Chunk<VertexFormat::Color> chunk(blockMap);

  EXPECT_TRUE(chunk.vertex.size() == 0);
}

TYPED_TEST(ChunkTest, InitChunkWithoutOpt) {
  const GLuint xSize = 2;
  const GLuint ySize = 2;
  const GLuint zSize = 2;
  const GLuint size = xSize * ySize * zSize;

  std::vector<GLuint> blockMap(xSize * ySize * zSize);
  std::fill(std::begin(blockMap), std::end(blockMap),
            static_cast<GLuint>(BlockType::Grass));

  using Chunk_T = Chunk<VertexFormat::Color>;
  Chunk_T::setSize({xSize, ySize, zSize});

  Chunk<VertexFormat::Color> chunk(blockMap);
  chunk.load();

  using MeshDataInfo_T = MeshData<Shape::Cube, VertexFormat::Color>::Info;
  const GLuint blockVertexSize = MeshDataInfo_T::vertexSize;

  for (GLuint i = 0; i < size; ++i) {
    std::span<GLfloat> chunkBlockVertex(
        chunk.vertex.data() + i * blockVertexSize, blockVertexSize);
    bool eq = std::equal(std::begin(chunkBlockVertex),
                         std::end(chunkBlockVertex), GrassBlockVertex);
    EXPECT_TRUE(eq);
  }
}
