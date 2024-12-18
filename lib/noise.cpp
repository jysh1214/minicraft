#include "noise.h"

#define NOISE_2D(NoiseTypeName, DataType)                                      \
  template <>                                                                  \
  void NoiseGenerator<NoiseType::NoiseTypeName>::noise2D(                      \
      std::span<DataType> data, const GLuint h, const GLuint w,                \
      const GLuint seed) {                                                     \
    if (data.size() != h * w)                                                  \
      handleError("NoiseGenerator::noise2D: Wrong data size.");                \
    FastNoiseLite noise;                                                       \
    noise.SetSeed(seed);                                                       \
    noise.SetNoiseType(FastNoiseLite::NoiseType_##NoiseTypeName);              \
    for (GLuint y = 0; y < h; ++y) {                                           \
      for (GLuint x = 0; x < w; ++x) {                                         \
        data[y * w + x] =                                                      \
            noise.GetNoise(static_cast<GLfloat>(x), static_cast<GLfloat>(y));  \
      }                                                                        \
    }                                                                          \
  }

NOISE_2D(OpenSimplex2, GLfloat)
NOISE_2D(OpenSimplex2S, GLfloat)
NOISE_2D(Cellular, GLfloat)
NOISE_2D(Perlin, GLfloat)
NOISE_2D(ValueCubic, GLfloat)
NOISE_2D(Value, GLfloat)

// OpenSimplex2,
// OpenSimplex2S,
// Cellular,
// Perlin,
// ValueCubic,
// Value,

/**
 *         NoiseType_OpenSimplex2,
        NoiseType_OpenSimplex2S,
        NoiseType_Cellular,
        NoiseType_Perlin,
        NoiseType_ValueCubic,
        NoiseType_Value
*/

// float GetNoise(FNfloat x, FNfloat y) const
// float GetNoise(FNfloat x, FNfloat y, FNfloat z) const