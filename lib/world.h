#ifndef MC_LIB_WORLD_H
#define MC_LIB_WORLD_H

#include <unordered_map>
#include <utility>
#include <vector>

#include <GL/glew.h>

#include "chunk.h"
#include "noise.h"
#include "terrain_generator.h"
#include "vertex_format.h"

// https://github.com/Cyan4973/xxHash/blob/dev/xxhash.h
// // Custom hash function for std::pair<int, int>
// struct pair_hash {
//     template <class T1, class T2>
//     std::size_t operator () (const std::pair<T1, T2>& p) const {
//         auto h1 = std::hash<T1>{}(p.first);
//         auto h2 = std::hash<T2>{}(p.second);
//         return h1 ^ h2; // Simple hash combining
//     }
// };

template <VertexFormat format> class World {
public:
  World() = default;
  ~World() = default;

  void setSize(const glm::uvec3 &worldSize);
  void setSeed(const GLuint seed);
  void init();

  // TEMP
  std::vector<Chunk<format>> chunks;

private:
  // TODO: Use xxHash to generate has value
  // std::unordered_map<std::pair<GLint, GLint>, Chunk<format>, CoorHash>
  // chunks;

  glm::uvec3 worldSize;

  GLuint seed = 76531;
};

#endif // MC_LIB_WORLD_H
