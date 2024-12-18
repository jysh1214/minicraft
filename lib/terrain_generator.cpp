#include "terrain_generator.h"

namespace {

void normalize(std::span<GLuint> out, std::span<GLfloat> in,
               const GLfloat height) {
  for (GLuint i = 0; i < in.size(); ++i) {
    out[i] = static_cast<GLuint>((in[i] + 1.0f) * 0.5 * height);
  }
}

} // namespace

template <>
void generateTerrain<NoiseType::OpenSimplex2>(std::span<GLuint> terrain,
                                              const GLuint xSize,
                                              const GLuint zSize,
                                              const GLuint height,
                                              const GLuint seed) {
  if (terrain.size() != xSize * zSize)
    handleError("NoiseGenerator::noise2D: Wrong data size.");

  std::vector<GLfloat> noise(xSize * zSize);
  NoiseGenerator<NoiseType::OpenSimplex2>::noise2D(noise, xSize, zSize, seed);

  ::normalize(terrain, noise, static_cast<GLfloat>(height));

  // TODO ocean?
  // more noise?
}
