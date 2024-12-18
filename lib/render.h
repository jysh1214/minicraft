#ifndef MC_LIB_RENDER_H
#define MC_LIB_RENDER_H

#include <span>

#include "block.h"
#include "camera.h"
#include "chunk.h"
#include "vertex_format.h"
#include "world.h"

template <VertexFormat format> struct Render {
  Render() {
    // Cull faces
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glFrontFace(GL_CW);

    // Show back
    // glEnable(GL_CULL_FACE);
    // glCullFace(GL_FRONT);
    // glFrontFace(GL_CW);
  }

  // NO copy constructor and copy assignment operator
  Render(const Render &other) = delete;
  Render &operator=(const Render &other) = delete;

  ~Render() = default;

  void draw(Camera &camera, Chunk<format> &chunk);
  void draw(Camera &camera, World<format> &world);
};

#endif // MC_LIB_RENDER_H
