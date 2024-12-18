#ifndef MC_LIB_MESH_H
#define MC_LIB_MESH_H

#include <vector>

#include <GL/glew.h> // gl type

#include "error_handler.h"
#include "vertex_info.h"

template <VertexFormat> struct Mesh {
  Mesh() = default;
  ~Mesh() = default;

  GLuint VAO, VBO, EBO;
  bool indexed = true;
  std::vector<GLfloat> vertices;
  std::vector<GLuint> indices;
};

#endif // MC_LIB_MESH_H
