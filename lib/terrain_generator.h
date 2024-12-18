#ifndef MC_LIB_TERRAIN_GENERATOR_H
#define MC_LIB_TERRAIN_GENERATOR_H

#include <span>
#include <vector>

#include <GL/glew.h>

#include "noise.h"

template <NoiseType noisetype>
void generateTerrain(std::span<GLuint> terrain, const GLuint xSize,
                     const GLuint zSize, const GLuint height,
                     const GLuint seed);

#endif // MC_LIB_TERRAIN_GENERATOR_H
