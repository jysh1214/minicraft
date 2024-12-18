#include "world.h"

namespace {

// We try to set world center as (0, 0)
std::tuple<GLint, GLint> getRange(const GLuint val) {
  return std::make_tuple<GLint, GLint>(-static_cast<GLint>(val / 2),
                                       static_cast<GLint>((val + 1) / 2));
}

} // namespace

// PosColor

template <>
void World<VertexFormat::PosColor>::setSize(const glm::uvec3 &worldSize) {
  this->worldSize = worldSize;
}

template <> void World<VertexFormat::PosColor>::setSeed(const GLuint seed) {
  this->seed = seed;
}

// // use cv mat to generate block map

template <> void World<VertexFormat::PosColor>::init() {
  // Generate noise
  glm::uvec3 chunkSize = Chunk<VertexFormat::PosColor>::getSize();
  std::vector<GLuint> terrain(this->worldSize.x * chunkSize.x *
                              this->worldSize.z * chunkSize.z);
  generateTerrain<NoiseType::OpenSimplex2>(
      terrain, this->worldSize.x * chunkSize.x, this->worldSize.z * chunkSize.z,
      this->worldSize.y * chunkSize.y, this->seed);

  const GLuint worldWide = this->worldSize.x * chunkSize.x;

  const GLuint chunkArea = chunkSize.x * chunkSize.z;
  const GLuint chunkWide = chunkSize.x;

  // TODO decide block type
  // we only use grass block now
  // std::vector<GLuint>

  // TODO: generate world size noise is not effiencient method
  // TODO: Apply bionism

  auto [startCoorX, endCoorX] =
      ::getRange(static_cast<GLint>(this->worldSize.x));
  auto [startCoorZ, endCoorZ] =
      ::getRange(static_cast<GLint>(this->worldSize.z));

  for (GLint cellY = 0; cellY < this->worldSize.y; ++cellY) {
    for (GLint cellZ = 0; cellZ < this->worldSize.z; ++cellZ) {
      for (GLint cellX = 0; cellX < this->worldSize.x; ++cellX) {

        const GLint startX = cellX * chunkSize.x;
        const GLint endX = startX + chunkSize.x;
        const GLint startY = cellY * chunkSize.y;
        const GLint endY = startY + chunkSize.y;
        const GLint startZ = cellZ * chunkSize.z;
        const GLint endZ = startZ + chunkSize.z;

        std::vector<GLuint> blockMap(chunkSize.x * chunkSize.y * chunkSize.z,
                                     0); // all air

        for (GLint tz = startZ; tz < endZ; ++tz) {
          for (GLint tx = startX; tx < endX; ++tx) {
            const GLint height =
                static_cast<GLint>(terrain[tz * worldWide + tx]);

            const GLint cx = tx - startX;
            const GLint cz = tz - startZ;
            for (GLuint cy = startY; cy < height; ++cy) {
              const GLuint chunkIndex =
                  (cy - startY) * chunkArea + cz * chunkSize.x + cx;
              blockMap[chunkIndex] = 1; // Grass
            }
          }
        }

        Chunk<VertexFormat::PosColor> chunk(blockMap);

        const glm::vec3 baseCoors{startX + startCoorX, startY,
                                  startZ + startCoorZ};
        chunk.setBaseCoors(baseCoors);

        this->chunks.push_back(chunk);

        // TODO: Apply bionism
      }
    }
  }
}

// PosTexCoor

template <>
void World<VertexFormat::PosTexCoor>::setSize(const glm::uvec3 &worldSize) {
  this->worldSize = worldSize;
}

template <> void World<VertexFormat::PosTexCoor>::setSeed(const GLuint seed) {
  this->seed = seed;
}

// // use cv mat to generate block map

template <> void World<VertexFormat::PosTexCoor>::init() {
  // Generate noise
  glm::uvec3 chunkSize = Chunk<VertexFormat::PosTexCoor>::getSize();
  std::vector<GLuint> terrain(this->worldSize.x * chunkSize.x *
                              this->worldSize.z * chunkSize.z);
  generateTerrain<NoiseType::OpenSimplex2>(
      terrain, this->worldSize.x * chunkSize.x, this->worldSize.z * chunkSize.z,
      this->worldSize.y * chunkSize.y, this->seed);

  const GLuint worldWide = this->worldSize.x * chunkSize.x;

  const GLuint chunkArea = chunkSize.x * chunkSize.z;
  const GLuint chunkWide = chunkSize.x;

  // TODO decide block type
  // we only use grass block now
  // std::vector<GLuint>

  // TODO: generate world size noise is not effiencient method
  // TODO: Apply bionism

  auto [startCoorX, endCoorX] =
      ::getRange(static_cast<GLint>(this->worldSize.x));
  auto [startCoorZ, endCoorZ] =
      ::getRange(static_cast<GLint>(this->worldSize.z));

  for (GLint cellY = 0; cellY < this->worldSize.y; ++cellY) {
    for (GLint cellZ = 0; cellZ < this->worldSize.z; ++cellZ) {
      for (GLint cellX = 0; cellX < this->worldSize.x; ++cellX) {

        const GLint startX = cellX * chunkSize.x;
        const GLint endX = startX + chunkSize.x;
        const GLint startY = cellY * chunkSize.y;
        const GLint endY = startY + chunkSize.y;
        const GLint startZ = cellZ * chunkSize.z;
        const GLint endZ = startZ + chunkSize.z;

        std::vector<GLuint> blockMap(chunkSize.x * chunkSize.y * chunkSize.z,
                                     0); // all air

        for (GLint tz = startZ; tz < endZ; ++tz) {
          for (GLint tx = startX; tx < endX; ++tx) {
            const GLint height =
                static_cast<GLint>(terrain[tz * worldWide + tx]);

            const GLint cx = tx - startX;
            const GLint cz = tz - startZ;
            for (GLuint cy = startY; cy < std::min(endY, height); ++cy) {
              const GLuint chunkIndex =
                  (cy - startY) * chunkArea + cz * chunkSize.x + cx;

              assert(chunkIndex < blockMap.size());
              blockMap[chunkIndex] = 1; // Grass
            }
          }
        }

        Chunk<VertexFormat::PosTexCoor> chunk(blockMap);

        const glm::vec3 baseCoors{startX + startCoorX, startY,
                                  startZ + startCoorZ};
        chunk.setBaseCoors(baseCoors);

        this->chunks.push_back(chunk);

        // TODO: Apply bionism
      }
    }
  }
}
