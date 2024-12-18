#ifndef MC_LIB_VERTEX_INFO_H
#define MC_LIB_VERTEX_INFO_H

#include <vector>

#include <GL/glew.h> // gl type

#include "vertex_format.h"

template <VertexFormat> struct VertexInfo {
  static const GLuint stride;
  static const std::vector<GLuint> attrSize;
};

#endif // MC_LIB_VERTEX_INFO_H
