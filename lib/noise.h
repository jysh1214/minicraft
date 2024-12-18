#ifndef MC_LIB_NOISE_H
#define MC_LIB_NOISE_H

#include <span>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/noise.hpp>

#include "FastNoiseLite.h"
#include "error_handler.h"

// clang-format off
enum class NoiseType {
  OpenSimplex2,
  OpenSimplex2S,
  Cellular,
  Perlin,
  ValueCubic,
  Value,
};
// clang-format on

// 1. whole world
// 2. connect chunk and blur the edge

template <NoiseType noiseType> struct NoiseGenerator {
  static void noise2D(std::span<GLfloat> data, const GLuint h, const GLuint w,
                      const GLuint seed);
  // static void noise3D();
};

#endif // MC_LIB_NOISE_H
