#include "chunk.h"

#define COPY_BACK_INSERT(Src, Dst)                                             \
  do {                                                                         \
    std::copy(std::begin(Src), std::end(Src), std::back_inserter(Dst));        \
  } while (0);

#define FACE_INDICES(FaceIndices, Offset, Index0, Index1, Index2, Index3,      \
                     Index4, Index5)                                           \
  std::vector<GLuint> FaceIndices{                                             \
      static_cast<GLuint>(Index0) + static_cast<GLuint>(Offset),               \
      static_cast<GLuint>(Index1) + static_cast<GLuint>(Offset),               \
      static_cast<GLuint>(Index2) + static_cast<GLuint>(Offset),               \
      static_cast<GLuint>(Index3) + static_cast<GLuint>(Offset),               \
      static_cast<GLuint>(Index4) + static_cast<GLuint>(Offset),               \
      static_cast<GLuint>(Index5) + static_cast<GLuint>(Offset)};

#define ASSIGN_ATTRIBUTES(VertexFormat)                                        \
  do {                                                                         \
    using VertexInfo_T = VertexInfo<VertexFormat>;                             \
    const GLuint stride = VertexInfo_T::stride;                                \
    const std::span<const GLuint> attrSize(std::begin(VertexInfo_T::attrSize), \
                                           VertexInfo_T::attrSize.size());     \
    GLuint offset = 0;                                                         \
    for (GLuint attrIndex = 0; attrIndex < attrSize.size(); ++attrIndex) {     \
      const GLuint size = attrSize[attrIndex];                                 \
      glVertexAttribPointer(attrIndex, size, GL_FLOAT, GL_FALSE,               \
                            stride * sizeof(GLfloat),                          \
                            (const void *)(offset * sizeof(GLfloat)));         \
      glEnableVertexAttribArray(attrIndex);                                    \
      offset += size;                                                          \
    }                                                                          \
  } while (0);

#define CHUNK_STATIC_MEMBER_METHOD_SPECIALIZATION(VertexFormat)                \
  template <> bool Chunk<VertexFormat>::isSizeSet = false;                     \
  template <> glm::uvec3 Chunk<VertexFormat>::chunkSize = glm::uvec3{1, 1, 1}; \
  template <> GLfloat Chunk<VertexFormat>::radius = 1.0f;                      \
  template <> void Chunk<VertexFormat>::setSize(const glm::uvec3 &size) {      \
    if (isSizeSet)                                                             \
      handleError("Chunk::setSize: The size has been set.");                   \
    if (size.x == 0 || size.y == 0 || size.z == 0)                             \
      handleError("Chunk::setSize: Please set size > 0.");                     \
    chunkSize = size;                                                          \
    isSizeSet = true;                                                          \
    radius = (static_cast<GLfloat>(chunkSize.x) * std::sqrt(3.0f)) / 2.0f;     \
  }                                                                            \
  template <> const glm::uvec3 Chunk<VertexFormat>::getSize() {                \
    return chunkSize;                                                          \
  }                                                                            \
  template <> GLfloat Chunk<VertexFormat>::getRadius() { return radius; }

CHUNK_STATIC_MEMBER_METHOD_SPECIALIZATION(VertexFormat::PosColor)
CHUNK_STATIC_MEMBER_METHOD_SPECIALIZATION(VertexFormat::PosTexCoor)
CHUNK_STATIC_MEMBER_METHOD_SPECIALIZATION(VertexFormat::PosFaceID)

namespace {

template <VertexFormat> struct FaceVertices;

template <> struct FaceVertices<VertexFormat::PosColor> {
  static std::vector<GLfloat>
  getData(const glm::vec3 &pos, const glm::vec4 &color, const GLfloat offset0,
          const GLfloat offset1, const GLfloat offset2, const GLfloat offset3,
          const GLfloat offset4, const GLfloat offset5, const GLfloat offset6,
          const GLfloat offset7, const GLfloat offset8, const GLfloat offset9,
          const GLfloat offset10, const GLfloat offset11) {
    return std::move(std::vector<GLfloat>{
        pos.x + offset0, pos.y + offset1,  pos.z + offset2,  color.x,
        color.y,         color.z,          color.w, // v0
        pos.x + offset3, pos.y + offset4,  pos.z + offset5,  color.x,
        color.y,         color.z,          color.w, // v1
        pos.x + offset6, pos.y + offset7,  pos.z + offset8,  color.x,
        color.y,         color.z,          color.w, // v2
        pos.x + offset9, pos.y + offset10, pos.z + offset11, color.x,
        color.y,         color.z,          color.w, // v3
    });
  }
};

template <> struct FaceVertices<VertexFormat::PosTexCoor> {
  static std::vector<GLfloat>
  getData(const glm::vec3 &pos, const glm::vec4 &color, const GLfloat offset0,
          const GLfloat offset1, const GLfloat offset2, const GLfloat offset3,
          const GLfloat offset4, const GLfloat offset5, const GLfloat offset6,
          const GLfloat offset7, const GLfloat offset8, const GLfloat offset9,
          const GLfloat offset10, const GLfloat offset11) {
    return std::move(std::vector<GLfloat>{
        pos.x + offset0, pos.y + offset1,  pos.z + offset2,  1, 1, // v0
        pos.x + offset3, pos.y + offset4,  pos.z + offset5,  1, 0, // v1
        pos.x + offset6, pos.y + offset7,  pos.z + offset8,  0, 0, // v2
        pos.x + offset9, pos.y + offset10, pos.z + offset11, 0, 1, // v3
    });
  }
};

inline GLuint blockIndexWithinChunk(const glm::uvec3 &pos,
                                    const glm::uvec3 &chunkSize) {
  return pos.y * (chunkSize.x * chunkSize.z) + pos.z * (chunkSize.x) + pos.x;
}

inline bool isBlockObscuring(const BlockType blockType) {
  return hasProperty<BlockProperty::PartialCell>(blockType) ||
         hasProperty<BlockProperty::Transparent>(blockType);
}

inline glm::vec3 uvec3_to_vec3(const glm::uvec3 &in) {
  return glm::vec3{static_cast<GLfloat>(in.x), static_cast<GLfloat>(in.y),
                   static_cast<GLfloat>(in.z)};
}

// Top face
//      v0(x, y+1, z) *------* v1(x+1, y+1, z)
//                   /      /
//                  /      /
// v3(x, y+1, z+1) *------* v2(x+1, y+1, z+1)
//
// Bottom face
//      v0(x, y, z) *------* v1(x+1, y, z)
//                 /      /
//                /      /
// v3(x, y, z+1) *------* v2(x+1, y, z+1)
//
// Right face
//      v1(x+1, y+1, z) *
//                     /|
//                    / |
// v2(x+1, y+1, z+1) *  * v0 (x+1, y, z)
//                   | /
//                   |/
//                   * v3(x+1, y, z+1)
//
// Left face
//      v1(x, y+1, z) *
//                   /|
//                  / |
// v2(x, y+1, z+1) *  * v0 (x, y, z)
//                 | /
//                 |/
//                 * v3(x, y, z+1)
//
// Back face
// v1(x, y+1, z)
//      *------* v2(x+1, y+1, z)
//      |      |
//      |      |
//      *------* v3(x+1, y, z)
// v0(x, y, z)
//
// Front face
// v1(x, y+1, z+1)
//      *------* v2(x+1, y+1, z+1)
//      |      |
//      |      |
//      *------* v3(x+1, y, z+1)
// v0(x, y, z+1)
template <VertexFormat>
void addVisibleFaces(std::vector<GLfloat> &vertices,
                     std::vector<GLuint> &indices,
                     const std::span<GLuint> blockMap,
                     const glm::vec3 &blockMapPos, const glm::uvec3 &chunkSize,
                     const glm::vec3 &worldBaseCoor, const glm::vec4 &color);

#if 0
#define ADD_VISIBLE_FACES_SPECIALIZATION(VertexFormat)                         \
  template <>                                                                  \
  void addVisibleFaces<VertexFormat>(                                          \
      std::vector<GLfloat> & vertices, std::vector<GLuint> & indices,          \
      const std::span<GLuint> blockMap, const glm::vec3 &blockMapPos,          \
      const glm::uvec3 &chunkSize, const glm::vec3 &pos,                       \
      const glm::vec4 &color) {                                                \
    const GLuint stride = VertexInfo<VertexFormat>::stride;                    \
    std::vector<GLfloat> topFaceVertices =                                     \
        FaceVertices<VertexFormat>::getData(pos, color, 0, 1, 0, 1, 1, 0, 1,   \
                                            1, 1, 0, 1, 1);                    \
    FACE_INDICES(topFaceIndices, vertices.size() / stride, 0, 3, 2, 0, 2, 1)   \
    if (blockMapPos.y == chunkSize.y - 1) {                                    \
      COPY_BACK_INSERT(topFaceVertices, vertices)                              \
      COPY_BACK_INSERT(topFaceIndices, indices)                                \
    } else {                                                                   \
      const glm::uvec3 aboveBlockPos = {blockMapPos.x, blockMapPos.y + 1,      \
                                        blockMapPos.z};                        \
      const GLuint aboveBlockIndex =                                           \
          blockIndexWithinChunk(aboveBlockPos, chunkSize);                     \
      if (isBlockObscuring(                                                    \
              static_cast<BlockType>(blockMap[aboveBlockIndex]))) {            \
        COPY_BACK_INSERT(topFaceVertices, vertices)                            \
        COPY_BACK_INSERT(topFaceIndices, indices)                              \
      }                                                                        \
    }                                                                          \
    std::vector<GLfloat> bottomFaceVertices =                                  \
        FaceVertices<VertexFormat>::getData(pos, color, 0, 0, 0, 1, 0, 0, 1,   \
                                            0, 1, 0, 0, 1);                    \
    FACE_INDICES(bottomFaceIndices, vertices.size() / stride, 0, 2, 3, 0, 1,   \
                 2)                                                            \
    if (blockMapPos.y == 0) {                                                  \
      COPY_BACK_INSERT(bottomFaceVertices, vertices)                           \
      COPY_BACK_INSERT(bottomFaceIndices, indices)                             \
    } else {                                                                   \
      const glm::uvec3 underBlockPos = {blockMapPos.x, blockMapPos.y - 1,      \
                                        blockMapPos.z};                        \
      const GLuint underBlockIndex =                                           \
          blockIndexWithinChunk(underBlockPos, chunkSize);                     \
      if (isBlockObscuring(                                                    \
              static_cast<BlockType>(blockMap[underBlockIndex]))) {            \
        COPY_BACK_INSERT(bottomFaceVertices, vertices)                         \
        COPY_BACK_INSERT(bottomFaceIndices, indices)                           \
      }                                                                        \
    }                                                                          \
    std::vector<GLfloat> rightFaceVertices =                                   \
        FaceVertices<VertexFormat>::getData(pos, color, 1, 0, 0, 1, 1, 0, 1,   \
                                            1, 1, 1, 0, 1);                    \
    FACE_INDICES(rightFaceIndices, vertices.size() / stride, 0, 1, 2, 0, 2, 3) \
    if (blockMapPos.x == chunkSize.x - 1) {                                    \
      COPY_BACK_INSERT(rightFaceVertices, vertices)                            \
      COPY_BACK_INSERT(rightFaceIndices, indices)                              \
    } else {                                                                   \
      const glm::uvec3 rightBlockPos = {blockMapPos.x + 1, blockMapPos.y,      \
                                        blockMapPos.z};                        \
      const GLuint rightBlockIndex =                                           \
          blockIndexWithinChunk(rightBlockPos, chunkSize);                     \
      if (isBlockObscuring(                                                    \
              static_cast<BlockType>(blockMap[rightBlockIndex]))) {            \
        COPY_BACK_INSERT(rightFaceVertices, vertices)                          \
        COPY_BACK_INSERT(rightFaceIndices, indices)                            \
      }                                                                        \
    }                                                                          \
    std::vector<GLfloat> leftFaceVertices =                                    \
        FaceVertices<VertexFormat>::getData(pos, color, 0, 0, 0, 0, 1, 0, 0,   \
                                            1, 1, 0, 0, 1);                    \
    FACE_INDICES(leftFaceIndices, vertices.size() / stride, 0, 2, 1, 0, 3, 2)  \
    if (blockMapPos.x == 0) {                                                  \
      COPY_BACK_INSERT(leftFaceVertices, vertices)                             \
      COPY_BACK_INSERT(leftFaceIndices, indices)                               \
    } else {                                                                   \
      const glm::uvec3 leftBlockPos = {blockMapPos.x - 1, blockMapPos.y,       \
                                       blockMapPos.z};                         \
      const GLuint leftBlockIndex =                                            \
          blockIndexWithinChunk(leftBlockPos, chunkSize);                      \
      if (isBlockObscuring(                                                    \
              static_cast<BlockType>(blockMap[leftBlockIndex]))) {             \
        COPY_BACK_INSERT(leftFaceVertices, vertices)                           \
        COPY_BACK_INSERT(leftFaceIndices, indices)                             \
      }                                                                        \
    }                                                                          \
    std::vector<GLfloat> backFaceVertices =                                    \
        FaceVertices<VertexFormat>::getData(pos, color, 0, 0, 0, 0, 1, 0, 1,   \
                                            1, 0, 1, 0, 0);                    \
    FACE_INDICES(backFaceIndices, vertices.size() / stride, 0, 1, 2, 0, 2, 3)  \
    if (blockMapPos.z == 0) {                                                  \
      COPY_BACK_INSERT(backFaceVertices, vertices)                             \
      COPY_BACK_INSERT(backFaceIndices, indices)                               \
    } else {                                                                   \
      const glm::uvec3 backBlockPos = {blockMapPos.x, blockMapPos.y,           \
                                       blockMapPos.z - 1};                     \
      const GLuint backBlockIndex =                                            \
          blockIndexWithinChunk(backBlockPos, chunkSize);                      \
      if (isBlockObscuring(                                                    \
              static_cast<BlockType>(blockMap[backBlockIndex]))) {             \
        COPY_BACK_INSERT(backFaceVertices, vertices)                           \
        COPY_BACK_INSERT(backFaceIndices, indices)                             \
      }                                                                        \
    }                                                                          \
    std::vector<GLfloat> frontFaceVertices =                                   \
        FaceVertices<VertexFormat>::getData(pos, color, 0, 0, 1, 0, 1, 1, 1,   \
                                            1, 1, 1, 0, 1);                    \
    FACE_INDICES(frontFaceIndices, vertices.size() / stride, 0, 2, 1, 0, 3, 2) \
    if (blockMapPos.z == chunkSize.z - 1) {                                    \
      COPY_BACK_INSERT(frontFaceVertices, vertices)                            \
      COPY_BACK_INSERT(frontFaceIndices, indices)                              \
    } else {                                                                   \
      const glm::uvec3 frontBlockPos = {blockMapPos.x, blockMapPos.y,          \
                                        blockMapPos.z + 1};                    \
      const GLuint frontBlockIndex =                                           \
          blockIndexWithinChunk(frontBlockPos, chunkSize);                     \
      if (isBlockObscuring(                                                    \
              static_cast<BlockType>(blockMap[frontBlockIndex]))) {            \
        COPY_BACK_INSERT(frontFaceVertices, vertices)                          \
        COPY_BACK_INSERT(frontFaceIndices, indices)                            \
      }                                                                        \
    }                                                                          \
  }

ADD_VISIBLE_FACES_SPECIALIZATION(VertexFormat::PosColor)
ADD_VISIBLE_FACES_SPECIALIZATION(VertexFormat::PosTexCoor)

#else

template <>
void addVisibleFaces<VertexFormat::PosColor>(std::vector<GLfloat> &vertices,
                                             std::vector<GLuint> &indices,
                                             const std::span<GLuint>
                                             blockMap, const glm::vec3
                                             &blockMapPos, const glm::uvec3
                                             &chunkSize, const glm::vec3
                                             &worldBaseCoor, const glm::vec4
                                             &color) {
  const glm::vec3 pos = worldBaseCoor;
  const GLuint stride = VertexInfo<VertexFormat::PosColor>::stride;
  const GLfloat topFaceVertices[] = {
      pos.x,     pos.y + 1, pos.z,     color.x, color.y, color.z, color.w, // v0
      pos.x + 1, pos.y + 1, pos.z,     color.x, color.y, color.z, color.w, // v1
      pos.x + 1, pos.y + 1, pos.z + 1, color.x, color.y, color.z, color.w, // v2
      pos.x,     pos.y + 1, pos.z + 1, color.x, color.y, color.z, color.w  // v3
  };
  FACE_INDICES(topFaceIndices, vertices.size() / stride, 0, 3, 2, 0, 2, 1)
  if (blockMapPos.y == chunkSize.y - 1) {
    COPY_BACK_INSERT(topFaceVertices, vertices)
    COPY_BACK_INSERT(topFaceIndices, indices)
  } else {
    const glm::uvec3 aboveBlockPos = {blockMapPos.x, blockMapPos.y + 1,
                                      blockMapPos.z};
    const GLuint aboveBlockIndex =
        blockIndexWithinChunk(aboveBlockPos, chunkSize);
    if (isBlockObscuring(static_cast<BlockType>(blockMap[aboveBlockIndex])))
    {
      COPY_BACK_INSERT(topFaceVertices, vertices)
      COPY_BACK_INSERT(topFaceIndices, indices)
    }
  }
  const GLfloat bottomFaceVertices[] = {
      pos.x,     pos.y, pos.z,     color.x, color.y, color.z, color.w, // v0
      pos.x + 1, pos.y, pos.z,     color.x, color.y, color.z, color.w, // v1
      pos.x + 1, pos.y, pos.z + 1, color.x, color.y, color.z, color.w, // v2
      pos.x,     pos.y, pos.z + 1, color.x, color.y, color.z, color.w  // v3
  };
  FACE_INDICES(bottomFaceIndices, vertices.size() / stride, 0, 2, 3, 0, 1, 2)
  if (blockMapPos.y == 0) {
    COPY_BACK_INSERT(bottomFaceVertices, vertices)
    COPY_BACK_INSERT(bottomFaceIndices, indices)
  } else {
    const glm::uvec3 underBlockPos = {blockMapPos.x, blockMapPos.y - 1,
                                      blockMapPos.z};
    const GLuint underBlockIndex =
        blockIndexWithinChunk(underBlockPos, chunkSize);
    if (isBlockObscuring(static_cast<BlockType>(blockMap[underBlockIndex])))
    {
      COPY_BACK_INSERT(bottomFaceVertices, vertices)
      COPY_BACK_INSERT(bottomFaceIndices, indices)
    }
  }
  const GLfloat rightFaceVertices[] = {
      pos.x + 1, pos.y,     pos.z,     color.x, color.y, color.z, color.w, // v0
      pos.x + 1, pos.y + 1, pos.z,     color.x, color.y, color.z, color.w, // v1
      pos.x + 1, pos.y + 1, pos.z + 1, color.x, color.y, color.z, color.w, // v2
      pos.x + 1, pos.y,     pos.z + 1, color.x, color.y, color.z, color.w  // v3
  };
  FACE_INDICES(rightFaceIndices, vertices.size() / stride, 0, 1, 2, 0, 2, 3)
  if (blockMapPos.x == chunkSize.x - 1) {
    COPY_BACK_INSERT(rightFaceVertices, vertices)
    COPY_BACK_INSERT(rightFaceIndices, indices)
  } else {
    const glm::uvec3 rightBlockPos = {blockMapPos.x + 1, blockMapPos.y,
                                      blockMapPos.z};
    const GLuint rightBlockIndex =
        blockIndexWithinChunk(rightBlockPos, chunkSize);
    if (isBlockObscuring(static_cast<BlockType>(blockMap[rightBlockIndex])))
    {
      COPY_BACK_INSERT(rightFaceVertices, vertices)
      COPY_BACK_INSERT(rightFaceIndices, indices)
    }
  }
  const GLfloat leftFaceVertices[] = {
      pos.x, pos.y,     pos.z,     color.x, color.y, color.z, color.w, // v0
      pos.x, pos.y + 1, pos.z,     color.x, color.y, color.z, color.w, // v1
      pos.x, pos.y + 1, pos.z + 1, color.x, color.y, color.z, color.w, // v2
      pos.x, pos.y,     pos.z + 1, color.x, color.y, color.z, color.w  // v3
  };
  FACE_INDICES(leftFaceIndices, vertices.size() / stride, 0, 2, 1, 0, 3, 2)
  if (blockMapPos.x == 0) {
    COPY_BACK_INSERT(leftFaceVertices, vertices)
    COPY_BACK_INSERT(leftFaceIndices, indices)
  } else {
    const glm::uvec3 leftBlockPos = {blockMapPos.x - 1, blockMapPos.y,
                                     blockMapPos.z};
    const GLuint leftBlockIndex =
        blockIndexWithinChunk(leftBlockPos, chunkSize);
    if (isBlockObscuring(static_cast<BlockType>(blockMap[leftBlockIndex]))) {
      COPY_BACK_INSERT(leftFaceVertices, vertices)
      COPY_BACK_INSERT(leftFaceIndices, indices)
    }
  }
  const GLfloat backFaceVertices[] = {
      pos.x,     pos.y,     pos.z, color.x, color.y, color.z, color.w, // v0
      pos.x,     pos.y + 1, pos.z, color.x, color.y, color.z, color.w, // v1
      pos.x + 1, pos.y + 1, pos.z, color.x, color.y, color.z, color.w, // v2
      pos.x + 1, pos.y,     pos.z, color.x, color.y, color.z, color.w  // v3
  };
  FACE_INDICES(backFaceIndices, vertices.size() / stride, 0, 1, 2, 0, 2, 3)
  if (blockMapPos.z == 0) {
    COPY_BACK_INSERT(backFaceVertices, vertices)
    COPY_BACK_INSERT(backFaceIndices, indices)
  } else {
    const glm::uvec3 backBlockPos = {blockMapPos.x, blockMapPos.y,
                                     blockMapPos.z - 1};
    const GLuint backBlockIndex =
        blockIndexWithinChunk(backBlockPos, chunkSize);
    if (isBlockObscuring(static_cast<BlockType>(blockMap[backBlockIndex]))) {
      COPY_BACK_INSERT(backFaceVertices, vertices)
      COPY_BACK_INSERT(backFaceIndices, indices)
    }
  }
  const GLfloat frontFaceVertices[] = {
      pos.x,     pos.y,     pos.z + 1, color.x, color.y, color.z, color.w, // v0
      pos.x,     pos.y + 1, pos.z + 1, color.x, color.y, color.z, color.w, // v1
      pos.x + 1, pos.y + 1, pos.z + 1, color.x, color.y, color.z, color.w, // v2
      pos.x + 1, pos.y,     pos.z + 1, color.x, color.y, color.z, color.w  // v3
  };
  FACE_INDICES(frontFaceIndices, vertices.size() / stride, 0, 2, 1, 0, 3, 2)
  if (blockMapPos.z == chunkSize.z - 1) {
    COPY_BACK_INSERT(frontFaceVertices, vertices)
    COPY_BACK_INSERT(frontFaceIndices, indices)
  } else {
    const glm::uvec3 frontBlockPos = {blockMapPos.x, blockMapPos.y,
                                      blockMapPos.z + 1};
    const GLuint frontBlockIndex =
        blockIndexWithinChunk(frontBlockPos, chunkSize);
    if (isBlockObscuring(static_cast<BlockType>(blockMap[frontBlockIndex])))
    {
      COPY_BACK_INSERT(frontFaceVertices, vertices)
      COPY_BACK_INSERT(frontFaceIndices, indices)
    }
  }
}

template <>
void addVisibleFaces<VertexFormat::PosTexCoor>(std::vector<GLfloat>
&vertices,
                                               std::vector<GLuint> &indices,
                                               const std::span<GLuint>
                                               blockMap, const glm::vec3
                                               &blockMapPos, const glm::uvec3
                                               &chunkSize, const glm::vec3
                                               &worldBaseCoor, const
                                               glm::vec4 &color) {
  const glm::vec3 pos = worldBaseCoor;
  const GLuint stride = VertexInfo<VertexFormat::PosTexCoor>::stride;

  // Top face
  //      v0(x, y+1, z) *------* v1(x+1, y+1, z)
  //                   /      /
  //                  /      /
  // v3(x, y+1, z+1) *------* v2(x+1, y+1, z+1)
  const GLfloat topFaceVertices[] = {
      pos.x,     pos.y + 1, pos.z,     0, 0, // v0
      pos.x + 1, pos.y + 1, pos.z,     1, 0, // v1
      pos.x + 1, pos.y + 1, pos.z + 1, 1, 1, // v2
      pos.x,     pos.y + 1, pos.z + 1, 0, 1, // v3
  };
  FACE_INDICES(topFaceIndices, vertices.size() / stride, 0, 3, 2, 0, 2, 1)
  if (blockMapPos.y == chunkSize.y - 1) {
    COPY_BACK_INSERT(topFaceVertices, vertices)
    COPY_BACK_INSERT(topFaceIndices, indices)
  } else {
    const glm::uvec3 aboveBlockPos = {blockMapPos.x, blockMapPos.y + 1,
                                      blockMapPos.z};
    const GLuint aboveBlockIndex =
        blockIndexWithinChunk(aboveBlockPos, chunkSize);
    if (isBlockObscuring(static_cast<BlockType>(blockMap[aboveBlockIndex])))
    {
      COPY_BACK_INSERT(topFaceVertices, vertices)
      COPY_BACK_INSERT(topFaceIndices, indices)
    }
  }

  // Bottom face
  //      v0(x, y, z) *------* v1(x+1, y, z)
  //                 /      /
  //                /      /
  // v3(x, y, z+1) *------* v2(x+1, y, z+1)
  const GLfloat bottomFaceVertices[] = {
      pos.x,     pos.y, pos.z,     0, 0, // v0
      pos.x + 1, pos.y, pos.z,     1, 0, // v1
      pos.x + 1, pos.y, pos.z + 1, 1, 1, // v2
      pos.x,     pos.y, pos.z + 1, 0, 1, // v3
  };
  FACE_INDICES(bottomFaceIndices, vertices.size() / stride, 0, 2, 3, 0, 1, 2)
  if (blockMapPos.y == 0) {
    COPY_BACK_INSERT(bottomFaceVertices, vertices)
    COPY_BACK_INSERT(bottomFaceIndices, indices)
  } else {
    const glm::uvec3 underBlockPos = {blockMapPos.x, blockMapPos.y - 1,
                                      blockMapPos.z};
    const GLuint underBlockIndex =
        blockIndexWithinChunk(underBlockPos, chunkSize);
    if (isBlockObscuring(static_cast<BlockType>(blockMap[underBlockIndex])))
    {
      COPY_BACK_INSERT(bottomFaceVertices, vertices)
      COPY_BACK_INSERT(bottomFaceIndices, indices)
    }
  }

  // Right face
  //      v1(x+1, y+1, z) *
  //                     /|
  //                    / |
  // v2(x+1, y+1, z+1) *  * v0 (x+1, y, z)
  //                   | /
  //                   |/
  //                   * v3(x+1, y, z+1)
  const GLfloat rightFaceVertices[] = {
      pos.x + 1, pos.y,     pos.z,     1, 1, // v0
      pos.x + 1, pos.y + 1, pos.z,     1, 0, // v1
      pos.x + 1, pos.y + 1, pos.z + 1, 0, 0, // v2
      pos.x + 1, pos.y,     pos.z + 1, 0, 1, // v3
  };
  FACE_INDICES(rightFaceIndices, vertices.size() / stride, 0, 1, 2, 0, 2, 3)
  if (blockMapPos.x == chunkSize.x - 1) {
    COPY_BACK_INSERT(rightFaceVertices, vertices)
    COPY_BACK_INSERT(rightFaceIndices, indices)
  } else {
    const glm::uvec3 rightBlockPos = {blockMapPos.x + 1, blockMapPos.y,
                                      blockMapPos.z};
    const GLuint rightBlockIndex =
        blockIndexWithinChunk(rightBlockPos, chunkSize);
    if (isBlockObscuring(static_cast<BlockType>(blockMap[rightBlockIndex])))
    {
      COPY_BACK_INSERT(rightFaceVertices, vertices)
      COPY_BACK_INSERT(rightFaceIndices, indices)
    }
  }

  // Left face
  //      v1(x, y+1, z) *
  //                   /|
  //                  / |
  // v2(x, y+1, z+1) *  * v0 (x, y, z)
  //                 | /
  //                 |/
  //                 * v3(x, y, z+1)
  const GLfloat leftFaceVertices[] = {
      pos.x, pos.y,     pos.z,     1, 1, // v0
      pos.x, pos.y + 1, pos.z,     1, 0, // v1
      pos.x, pos.y + 1, pos.z + 1, 0, 0, // v2
      pos.x, pos.y,     pos.z + 1, 0, 1, // v3
  };
  FACE_INDICES(leftFaceIndices, vertices.size() / stride, 0, 2, 1, 0, 3, 2)
  if (blockMapPos.x == 0) {
    COPY_BACK_INSERT(leftFaceVertices, vertices)
    COPY_BACK_INSERT(leftFaceIndices, indices)
  } else {
    const glm::uvec3 leftBlockPos = {blockMapPos.x - 1, blockMapPos.y,
                                     blockMapPos.z};
    const GLuint leftBlockIndex =
        blockIndexWithinChunk(leftBlockPos, chunkSize);
    if (isBlockObscuring(static_cast<BlockType>(blockMap[leftBlockIndex]))) {
      COPY_BACK_INSERT(leftFaceVertices, vertices)
      COPY_BACK_INSERT(leftFaceIndices, indices)
    }
  }

  // Back face
  // v1(x, y+1, z)
  //      *------* v2(x+1, y+1, z)
  //      |      |
  //      |      |
  //      *------* v3(x+1, y, z)
  // v0(x, y, z)
  const GLfloat backFaceVertices[] = {
      pos.x,     pos.y,     pos.z, 0, 1, // v0
      pos.x,     pos.y + 1, pos.z, 0, 0, // v1
      pos.x + 1, pos.y + 1, pos.z, 1, 0, // v2
      pos.x + 1, pos.y,     pos.z, 1, 1, // v3
  };
  FACE_INDICES(backFaceIndices, vertices.size() / stride, 0, 1, 2, 0, 2, 3)
  if (blockMapPos.z == 0) {
    COPY_BACK_INSERT(backFaceVertices, vertices)
    COPY_BACK_INSERT(backFaceIndices, indices)
  } else {
    const glm::uvec3 backBlockPos = {blockMapPos.x, blockMapPos.y,
                                     blockMapPos.z - 1};
    const GLuint backBlockIndex =
        blockIndexWithinChunk(backBlockPos, chunkSize);
    if (isBlockObscuring(static_cast<BlockType>(blockMap[backBlockIndex]))) {
      COPY_BACK_INSERT(backFaceVertices, vertices)
      COPY_BACK_INSERT(backFaceIndices, indices)
    }
  }

  // Front face
  // v1(x, y+1, z+1)
  //      *------* v2(x+1, y+1, z+1)
  //      |      |
  //      |      |
  //      *------* v3(x+1, y, z+1)
  // v0(x, y, z+1)
  const GLfloat frontFaceVertices[] = {
      pos.x,     pos.y,     pos.z + 1, 0, 1, // v0
      pos.x,     pos.y + 1, pos.z + 1, 0, 0, // v1
      pos.x + 1, pos.y + 1, pos.z + 1, 1, 0, // v2
      pos.x + 1, pos.y,     pos.z + 1, 1, 1, // v3
  };
  FACE_INDICES(frontFaceIndices, vertices.size() / stride, 0, 2, 1, 0, 3, 2)
  if (blockMapPos.z == chunkSize.z - 1) {
    COPY_BACK_INSERT(frontFaceVertices, vertices)
    COPY_BACK_INSERT(frontFaceIndices, indices)
  } else {
    const glm::uvec3 frontBlockPos = {blockMapPos.x, blockMapPos.y,
                                      blockMapPos.z + 1};
    const GLuint frontBlockIndex =
        blockIndexWithinChunk(frontBlockPos, chunkSize);
    if (isBlockObscuring(static_cast<BlockType>(blockMap[frontBlockIndex])))
    {
      COPY_BACK_INSERT(frontFaceVertices, vertices)
      COPY_BACK_INSERT(frontFaceIndices, indices)
    }
  }
}

/**
 * TOP    0
 * BOTTOM 1
 * RIGHT  2
 * LEFT   3
 * BACK   4
 * FRONT  5 
*/
template <>
void addVisibleFaces<VertexFormat::PosFaceID>(std::vector<GLfloat>
&vertices,
                                               std::vector<GLuint> &indices,
                                               const std::span<GLuint>
                                               blockMap, const glm::vec3
                                               &blockMapPos, const glm::uvec3
                                               &chunkSize, const glm::vec3
                                               &worldBaseCoor, const
                                               glm::vec4 &color) {
  const glm::vec3 pos = worldBaseCoor;
  const GLuint stride = VertexInfo<VertexFormat::PosFaceID>::stride;

  // Top face
  //      v0(x, y+1, z) *------* v1(x+1, y+1, z)
  //                   /      /
  //                  /      /
  // v3(x, y+1, z+1) *------* v2(x+1, y+1, z+1)
  const GLfloat topFaceVertices[] = {
      pos.x,     pos.y + 1, pos.z,     0, // v0
      pos.x + 1, pos.y + 1, pos.z,     0, // v1
      pos.x + 1, pos.y + 1, pos.z + 1, 0, // v2
      pos.x,     pos.y + 1, pos.z + 1, 0, // v3
  };
  FACE_INDICES(topFaceIndices, vertices.size() / stride, 0, 3, 2, 0, 2, 1)
  if (blockMapPos.y == chunkSize.y - 1) {
    COPY_BACK_INSERT(topFaceVertices, vertices)
    COPY_BACK_INSERT(topFaceIndices, indices)
  } else {
    const glm::uvec3 aboveBlockPos = {blockMapPos.x, blockMapPos.y + 1,
                                      blockMapPos.z};
    const GLuint aboveBlockIndex =
        blockIndexWithinChunk(aboveBlockPos, chunkSize);
    if (isBlockObscuring(static_cast<BlockType>(blockMap[aboveBlockIndex])))
    {
      COPY_BACK_INSERT(topFaceVertices, vertices)
      COPY_BACK_INSERT(topFaceIndices, indices)
    }
  }

  // Bottom face
  //      v0(x, y, z) *------* v1(x+1, y, z)
  //                 /      /
  //                /      /
  // v3(x, y, z+1) *------* v2(x+1, y, z+1)
  const GLfloat bottomFaceVertices[] = {
      pos.x,     pos.y, pos.z,     1, // v0
      pos.x + 1, pos.y, pos.z,     1, // v1
      pos.x + 1, pos.y, pos.z + 1, 1, // v2
      pos.x,     pos.y, pos.z + 1, 1, // v3
  };
  FACE_INDICES(bottomFaceIndices, vertices.size() / stride, 0, 2, 3, 0, 1, 2)
  if (blockMapPos.y == 0) {
    COPY_BACK_INSERT(bottomFaceVertices, vertices)
    COPY_BACK_INSERT(bottomFaceIndices, indices)
  } else {
    const glm::uvec3 underBlockPos = {blockMapPos.x, blockMapPos.y - 1,
                                      blockMapPos.z};
    const GLuint underBlockIndex =
        blockIndexWithinChunk(underBlockPos, chunkSize);
    if (isBlockObscuring(static_cast<BlockType>(blockMap[underBlockIndex])))
    {
      COPY_BACK_INSERT(bottomFaceVertices, vertices)
      COPY_BACK_INSERT(bottomFaceIndices, indices)
    }
  }

  // Right face
  //      v1(x+1, y+1, z) *
  //                     /|
  //                    / |
  // v2(x+1, y+1, z+1) *  * v0 (x+1, y, z)
  //                   | /
  //                   |/
  //                   * v3(x+1, y, z+1)
  const GLfloat rightFaceVertices[] = {
      pos.x + 1, pos.y,     pos.z,     2, // v0
      pos.x + 1, pos.y + 1, pos.z,     2, // v1
      pos.x + 1, pos.y + 1, pos.z + 1, 2, // v2
      pos.x + 1, pos.y,     pos.z + 1, 2, // v3
  };
  FACE_INDICES(rightFaceIndices, vertices.size() / stride, 0, 1, 2, 0, 2, 3)
  if (blockMapPos.x == chunkSize.x - 1) {
    COPY_BACK_INSERT(rightFaceVertices, vertices)
    COPY_BACK_INSERT(rightFaceIndices, indices)
  } else {
    const glm::uvec3 rightBlockPos = {blockMapPos.x + 1, blockMapPos.y,
                                      blockMapPos.z};
    const GLuint rightBlockIndex =
        blockIndexWithinChunk(rightBlockPos, chunkSize);
    if (isBlockObscuring(static_cast<BlockType>(blockMap[rightBlockIndex])))
    {
      COPY_BACK_INSERT(rightFaceVertices, vertices)
      COPY_BACK_INSERT(rightFaceIndices, indices)
    }
  }

  // Left face
  //      v1(x, y+1, z) *
  //                   /|
  //                  / |
  // v2(x, y+1, z+1) *  * v0 (x, y, z)
  //                 | /
  //                 |/
  //                 * v3(x, y, z+1)
  const GLfloat leftFaceVertices[] = {
      pos.x, pos.y,     pos.z,     3, // v0
      pos.x, pos.y + 1, pos.z,     3, // v1
      pos.x, pos.y + 1, pos.z + 1, 3, // v2
      pos.x, pos.y,     pos.z + 1, 3, // v3
  };
  FACE_INDICES(leftFaceIndices, vertices.size() / stride, 0, 2, 1, 0, 3, 2)
  if (blockMapPos.x == 0) {
    COPY_BACK_INSERT(leftFaceVertices, vertices)
    COPY_BACK_INSERT(leftFaceIndices, indices)
  } else {
    const glm::uvec3 leftBlockPos = {blockMapPos.x - 1, blockMapPos.y,
                                     blockMapPos.z};
    const GLuint leftBlockIndex =
        blockIndexWithinChunk(leftBlockPos, chunkSize);
    if (isBlockObscuring(static_cast<BlockType>(blockMap[leftBlockIndex]))) {
      COPY_BACK_INSERT(leftFaceVertices, vertices)
      COPY_BACK_INSERT(leftFaceIndices, indices)
    }
  }

  // Back face
  // v1(x, y+1, z)
  //      *------* v2(x+1, y+1, z)
  //      |      |
  //      |      |
  //      *------* v3(x+1, y, z)
  // v0(x, y, z)
  const GLfloat backFaceVertices[] = {
      pos.x,     pos.y,     pos.z, 4, // v0
      pos.x,     pos.y + 1, pos.z, 4, // v1
      pos.x + 1, pos.y + 1, pos.z, 4, // v2
      pos.x + 1, pos.y,     pos.z, 4, // v3
  };
  FACE_INDICES(backFaceIndices, vertices.size() / stride, 0, 1, 2, 0, 2, 3)
  if (blockMapPos.z == 0) {
    COPY_BACK_INSERT(backFaceVertices, vertices)
    COPY_BACK_INSERT(backFaceIndices, indices)
  } else {
    const glm::uvec3 backBlockPos = {blockMapPos.x, blockMapPos.y,
                                     blockMapPos.z - 1};
    const GLuint backBlockIndex =
        blockIndexWithinChunk(backBlockPos, chunkSize);
    if (isBlockObscuring(static_cast<BlockType>(blockMap[backBlockIndex]))) {
      COPY_BACK_INSERT(backFaceVertices, vertices)
      COPY_BACK_INSERT(backFaceIndices, indices)
    }
  }

  // Front face
  // v1(x, y+1, z+1)
  //      *------* v2(x+1, y+1, z+1)
  //      |      |
  //      |      |
  //      *------* v3(x+1, y, z+1)
  // v0(x, y, z+1)
  const GLfloat frontFaceVertices[] = {
      pos.x,     pos.y,     pos.z + 1, 5, // v0
      pos.x,     pos.y + 1, pos.z + 1, 5, // v1
      pos.x + 1, pos.y + 1, pos.z + 1, 5, // v2
      pos.x + 1, pos.y,     pos.z + 1, 5, // v3
  };
  FACE_INDICES(frontFaceIndices, vertices.size() / stride, 0, 2, 1, 0, 3, 2)
  if (blockMapPos.z == chunkSize.z - 1) {
    COPY_BACK_INSERT(frontFaceVertices, vertices)
    COPY_BACK_INSERT(frontFaceIndices, indices)
  } else {
    const glm::uvec3 frontBlockPos = {blockMapPos.x, blockMapPos.y,
                                      blockMapPos.z + 1};
    const GLuint frontBlockIndex =
        blockIndexWithinChunk(frontBlockPos, chunkSize);
    if (isBlockObscuring(static_cast<BlockType>(blockMap[frontBlockIndex])))
    {
      COPY_BACK_INSERT(frontFaceVertices, vertices)
      COPY_BACK_INSERT(frontFaceIndices, indices)
    }
  }
}

#endif

inline const GLuint divFloor(const GLuint a, const GLuint b) {
  assert(b != 0.0f);
  return static_cast<GLuint>(
      std::floor(static_cast<GLfloat>(a) / static_cast<GLfloat>(b)));
}

inline const glm::vec3 calcBlockCoordinates(const GLuint index,
                                            const glm::uvec3 chunkSize,
                                            const GLuint horizontalArea) {
  const GLuint y = ::divFloor(index, horizontalArea);
  const GLuint rest = index - (y * horizontalArea);
  const GLuint z = ::divFloor(rest, chunkSize.x);
  const GLuint x = rest % chunkSize.x;
  return {static_cast<GLfloat>(x), static_cast<GLfloat>(y),
          static_cast<GLfloat>(z)};
}

// template <typename Container_T>
// concept Insertable =
//     requires(Container_T &container, typename Container_T::value_type val,
//              typename Container_T::const_iterator pos) {
//       {
//         container.insert(pos, val)
//         } -> std::same_as<typename Container_T::iterator>;
//     };

// template <typename Container_T, VertexFormat format>
//   requires Insertable<Container_T>
template <VertexFormat>
void createBlockByMap(std::vector<GLfloat> &vertices,
                      std::vector<GLuint> &indices,
                      const glm::vec3 &chunkBaseCoors,
                      const std::span<GLuint> blockMap);

#define CREATE_BLOCK_BY_MAP_SPECAILIZATION(VertexFormat) \
template <> \
void createBlockByMap<VertexFormat>( \
    std::vector<GLfloat> &vertices, std::vector<GLuint> &indices, \
    const glm::vec3 &chunkBaseCoors, const std::span<GLuint> blockMap) { \
  const glm::uvec3 chunkSize = Chunk<VertexFormat>::getSize(); \
  const GLuint horizontalArea = chunkSize.x * chunkSize.z; \
  for (GLuint blockIndex = 0; blockIndex < blockMap.size(); ++blockIndex) { \
    if (static_cast<BlockType>(blockMap[blockIndex]) == BlockType::Air) \
      continue; \
    const glm::vec3 blockCoors = \
        ::calcBlockCoordinates(blockIndex, chunkSize, horizontalArea); \
    const glm::vec3 worldBaseCoors = blockCoors + chunkBaseCoors; \
    const glm::vec4 color = \
        getBlockColor(static_cast<BlockType>(blockMap[blockIndex])); \
    ::addVisibleFaces<VertexFormat>(vertices, indices, blockMap, \
                                              blockCoors, chunkSize, \
                                              worldBaseCoors, color); \
  } \
}

CREATE_BLOCK_BY_MAP_SPECAILIZATION(VertexFormat::PosColor)
CREATE_BLOCK_BY_MAP_SPECAILIZATION(VertexFormat::PosTexCoor)
CREATE_BLOCK_BY_MAP_SPECAILIZATION(VertexFormat::PosFaceID)

// // VertexFormat::PosColor
// template <>
// void createBlockByMap<VertexFormat::PosColor>(
//     std::vector<GLfloat> &vertices, std::vector<GLuint> &indices,
//     const glm::vec3 &chunkBaseCoors, const std::span<GLuint> blockMap) {

//   const glm::uvec3 chunkSize = Chunk<VertexFormat::PosColor>::getSize();
//   const GLuint horizontalArea = chunkSize.x * chunkSize.z;

//   for (GLuint blockIndex = 0; blockIndex < blockMap.size(); ++blockIndex) {
//     if (static_cast<BlockType>(blockMap[blockIndex]) == BlockType::Air)
//       continue;

//     const glm::vec3 blockCoors =
//         ::calcBlockCoordinates(blockIndex, chunkSize, horizontalArea);
//     const glm::vec3 worldBaseCoors = blockCoors + chunkBaseCoors;

//     const glm::vec4 color =
//         getBlockColor(static_cast<BlockType>(blockMap[blockIndex]));

//     // occlusion culling
//     ::addVisibleFaces<VertexFormat::PosColor>(vertices, indices, blockMap,
//                                               blockCoors, chunkSize,
//                                               worldBaseCoors, color);
//   }
// }

// // VertexFormat::PosTexCoor
// template <>
// void createBlockByMap<VertexFormat::PosTexCoor>(
//     std::vector<GLfloat> &vertices, std::vector<GLuint> &indices,
//     const glm::vec3 &chunkBaseCoors, const std::span<GLuint> blockMap) {

//   const glm::uvec3 chunkSize = Chunk<VertexFormat::PosTexCoor>::getSize();
//   const GLuint horizontalArea = chunkSize.x * chunkSize.z;

//   for (GLuint blockIndex = 0; blockIndex < blockMap.size(); ++blockIndex) {
//     if (static_cast<BlockType>(blockMap[blockIndex]) == BlockType::Air)
//       continue;

//     const glm::vec3 blockCoors =
//         ::calcBlockCoordinates(blockIndex, chunkSize, horizontalArea);
//     const glm::vec3 worldBaseCoors = blockCoors + chunkBaseCoors;

//     const glm::vec4 color =
//         getBlockColor(static_cast<BlockType>(blockMap[blockIndex]));

//     // occlusion culling
//     ::addVisibleFaces<VertexFormat::PosTexCoor>(vertices, indices, blockMap,
//                                                 blockCoors, chunkSize,
//                                                 worldBaseCoors, color);
//   }
// }

} // namespace

// #define CHUNK_SPECIALIZATION(VertexFormat)                                     \
//   template <> Chunk<VertexFormat>::Chunk(const std::span<GLuint> inMap) {      \
//     if (!isSizeSet) {                                                          \
//       handleError(                                                             \
//           "Chunk constructor: Please set the chunk size first. (Using "        \
//           "`Chunk::setSize`)");                                                \
//     }                                                                          \
//     const GLuint mapSize = chunkSize.x * chunkSize.y * chunkSize.z;            \
//     if (inMap.size() != mapSize) {                                             \
//       handleError("Chunk constructor: `inMap` has illegal size.");             \
//     }                                                                          \
//     this->loaded = false;                                                      \
//     this->blockMap = std::vector<GLuint>(std::begin(inMap), std::end(inMap));  \
//   }                                                                            \
//   template <>                                                                  \
//   void Chunk<VertexFormat>::setBaseCoors(const glm::vec3 &chunkBaseCoors) {    \
//     this->baseCoors = chunkBaseCoors;                                          \
//     this->center = this->baseCoors +                                           \
//                    (uvec3_to_vec3(Chunk<VertexFormat>::getSize()) / 2.0f);     \
//   }                                                                            \
//   template <> const glm::vec3 Chunk<VertexFormat>::getCenter() const {         \
//     return this->center;                                                       \
//   }                                                                            \
//   template <> void Chunk<VertexFormat>::load() {                               \
//     this->loaded = true;                                                       \
//     this->vertices.clear();                                                    \
//     this->indices.clear();                                                     \
//     ::createBlockByMap<VertexFormat>(this->vertices, this->indices,            \
//                                      this->baseCoors, this->blockMap);         \
//     glGenVertexArrays(1, &(this->VAO));                                        \
//     glBindVertexArray(this->VAO);                                              \
//     glGenBuffers(1, &(this->VBO));                                             \
//     glBindBuffer(GL_ARRAY_BUFFER, this->VBO);                                  \
//     glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(GLfloat),     \
//                  this->vertices.data(), GL_STATIC_DRAW);                       \
//     if (this->indexed) {                                                       \
//       glGenBuffers(1, &(this->EBO));                                           \
//       glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);                        \
//       glBufferData(GL_ELEMENT_ARRAY_BUFFER,                                    \
//                    this->indices.size() * sizeof(GLuint),                      \
//                    this->indices.data(), GL_STATIC_DRAW);                      \
//     }                                                                          \
//     if (textured) {                                                            \
//       glGenTextures(1, &(this->texture));                                      \
//       glBindTexture(GL_TEXTURE_2D, this->texture);                             \
//       glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);            \
//       glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);            \
//       glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);        \
//       glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);        \
//       cv::Mat image =                                                          \
//           cv::imread("/Users/alex/minicraft/assets/cute_400x400.png");         \
//       if (image.empty()) {                                                     \
//         handleError("Failed to load texture.");                                \
//       }                                                                        \
//       cv::cvtColor(image, image, cv::COLOR_BGR2RGB);                           \
//       glGenTextures(1, &(this->texture));                                      \
//       glBindTexture(GL_TEXTURE_2D, this->texture);                             \
//       glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);     \
//       glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);     \
//       glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);        \
//       glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);        \
//       glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.cols, image.rows, 0,        \
//                    GL_RGB, GL_UNSIGNED_BYTE, image.data);                      \
//       glGenerateMipmap(GL_TEXTURE_2D);                                         \
//     }                                                                          \
//     ASSIGN_ATTRIBUTES(VertexFormat)                                            \
//     glBindBuffer(GL_ARRAY_BUFFER, 0);                                          \
//     glBindVertexArray(0);                                                      \
//   }                                                                            \
//   template <> void Chunk<VertexFormat>::unload() {                             \
//     glDeleteTextures(1, &texture);                                             \
//     this->loaded = false;                                                      \
//     this->vertices.clear();                                                    \
//     this->vertices.reserve(0);                                                 \
//   }                                                                            \
//   template <> bool Chunk<VertexFormat>::isLoaded() { return this->loaded; }    \
//   template <> void Chunk<VertexFormat>::render() {                             \
//     if (!this->isLoaded()) {                                                   \
//       this->load();                                                            \
//     }                                                                          \
//     glBindVertexArray(this->VAO);                                              \
//     if (textured) {                                                            \
//       glActiveTexture(GL_TEXTURE0);                                            \
//       glBindTexture(GL_TEXTURE_2D, texture);                                   \
//     }                                                                          \
//     this->indexed ? glDrawElements(GL_TRIANGLES, this->indices.size(),         \
//                                    GL_UNSIGNED_INT, 0)                         \
//                   : glDrawArrays(GL_TRIANGLES, 0,                              \
//                                  this->vertices.size() /                       \
//                                      VertexInfo<VertexFormat>::stride);        \
//     glBindVertexArray(0);                                                      \
//   }

#define CHUNK_SPECIALIZATION(VertexFormat)                                     \
  template <> Chunk<VertexFormat>::Chunk(const std::span<GLuint> inMap) {      \
    if (!isSizeSet) {                                                          \
      handleError(                                                             \
          "Chunk constructor: Please set the chunk size first. (Using "        \
          "`Chunk::setSize`)");                                                \
    }                                                                          \
    const GLuint mapSize = chunkSize.x * chunkSize.y * chunkSize.z;            \
    if (inMap.size() != mapSize) {                                             \
      handleError("Chunk constructor: `inMap` has illegal size.");             \
    }                                                                          \
    this->loaded = false;                                                      \
    this->blockMap = std::vector<GLuint>(std::begin(inMap), std::end(inMap));  \
  }                                                                            \
  template <>                                                                  \
  void Chunk<VertexFormat>::setBaseCoors(const glm::vec3 &chunkBaseCoors) {    \
    this->baseCoors = chunkBaseCoors;                                          \
    this->center = this->baseCoors +                                           \
                   (uvec3_to_vec3(Chunk<VertexFormat>::getSize()) / 2.0f);     \
  }                                                                            \
  template <> const glm::vec3 Chunk<VertexFormat>::getCenter() const {         \
    return this->center;                                                       \
  }                                                                            \
  template <> void Chunk<VertexFormat>::load() {                               \
    this->loaded = true;                                                       \
    this->vertices.clear();                                                    \
    this->indices.clear();                                                     \
    ::createBlockByMap<VertexFormat>(this->vertices, this->indices,            \
                                     this->baseCoors, this->blockMap);         \
    glGenVertexArrays(1, &(this->VAO));                                        \
    glBindVertexArray(this->VAO);                                              \
    glGenBuffers(1, &(this->VBO));                                             \
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);                                  \
    glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(GLfloat),     \
                 this->vertices.data(), GL_STATIC_DRAW);                       \
    if (this->indexed) {                                                       \
      glGenBuffers(1, &(this->EBO));                                           \
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);                        \
      glBufferData(GL_ELEMENT_ARRAY_BUFFER,                                    \
                   this->indices.size() * sizeof(GLuint),                      \
                   this->indices.data(), GL_STATIC_DRAW);                      \
    }                                                                          \
    if (textured) {                                                            \
      cv::Mat image =                                                          \
          cv::imread("/Users/alex/minicraft/tex.png");                      \
      if (image.empty()) {                                                     \
        handleError("Failed to load texture.");                                \
      }                                                                        \
      cv::cvtColor(image, image, cv::COLOR_BGR2RGB);                           \
      glGenTextures(1, &(this->texture));                                      \
      glBindTexture(GL_TEXTURE_2D, this->texture);                             \
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);     \
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);     \
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);        \
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);        \
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.cols, image.rows, 0,        \
                   GL_RGB, GL_UNSIGNED_BYTE, image.data);                      \
      glGenerateMipmap(GL_TEXTURE_2D);                                         \
    }                                                                          \
    ASSIGN_ATTRIBUTES(VertexFormat)                                            \
    glBindBuffer(GL_ARRAY_BUFFER, 0);                                          \
    glBindVertexArray(0);                                                      \
  }                                                                            \
  template <> void Chunk<VertexFormat>::unload() {                             \
    glDeleteTextures(1, &texture);                                             \
    this->loaded = false;                                                      \
    this->vertices.clear();                                                    \
    this->vertices.reserve(0);                                                 \
  }                                                                            \
  template <> bool Chunk<VertexFormat>::isLoaded() { return this->loaded; }    \
  template <> void Chunk<VertexFormat>::render() {                             \
    if (!this->isLoaded()) {                                                   \
      this->load();                                                            \
    }                                                                          \
    glBindVertexArray(this->VAO);                                              \
    if (textured) {                                                            \
      glActiveTexture(GL_TEXTURE0);                                            \
      glBindTexture(GL_TEXTURE_2D, texture);                                   \
    }                                                                          \
    this->indexed ? glDrawElements(GL_TRIANGLES, this->indices.size(),         \
                                   GL_UNSIGNED_INT, 0)                         \
                  : glDrawArrays(GL_TRIANGLES, 0,                              \
                                 this->vertices.size() /                       \
                                     VertexInfo<VertexFormat>::stride);        \
    glBindVertexArray(0);                                                      \
  }

CHUNK_SPECIALIZATION(VertexFormat::PosColor)
CHUNK_SPECIALIZATION(VertexFormat::PosTexCoor)
CHUNK_SPECIALIZATION(VertexFormat::PosFaceID)
