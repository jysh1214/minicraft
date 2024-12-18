#ifndef MC_LIB_CHUNK_H
#define MC_LIB_CHUNK_H

#include <algorithm>
#include <cmath>
#include <iterator>
#include <span>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <opencv2/opencv.hpp>

#include "block.h"
#include "color.h"
#include "mesh.h"
#include "optimization.h"
#include "vertex_format.h"

template <VertexFormat format> class Chunk : public Mesh<format> {
public:
  static void setSize(const glm::uvec3 &size);
  static const glm::uvec3 getSize();
  static GLfloat getRadius();

  Chunk(const std::span<GLuint> inMap);
  ~Chunk() = default;

  void setBaseCoors(const glm::vec3 &chunkBaseCoors);
  const glm::vec3 getCenter() const;
  // lazy allocate: util the chunk is visible
  void load();
  void unload();
  bool isLoaded();
  void render();

private:
  static bool isSizeSet;
  static glm::uvec3 chunkSize;
  static GLfloat radius;
  static constexpr bool textured = (format == VertexFormat::PosTexCoor) ||
                                   (format == VertexFormat::PosColorTexCoor) ||
                                   (format == VertexFormat::PosFaceID);

  std::vector<GLuint> blockMap;
  GLuint VAO, VBO, EBO;
  GLuint texture;
  glm::vec3 baseCoors{0.0f, 0.0f, 0.0f};
  bool loaded = false;
  glm::vec3 center{0.0f, 0.0f, 0.0f};
};

#endif // MC_LIB_CHUNK_H
