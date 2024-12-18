#ifndef MC_LIB_DEBUG_UTILS_H
#define MC_LIB_DEBUG_UTILS_H

#include <iostream>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#undef GLM_ENABLE_EXPERIMENTAL

template <class T>
void debug_print(T *data, const size_t col, const size_t row) {
  for (size_t j = 0; j < row; ++j) {
    for (size_t i = 0; i < col; ++i) {
      std::cerr << data[j * col + i];
      if (i < col - 1)
        std::cerr << ", ";
    }
    std::cerr << std::endl;
  }
}

#endif // MC_LIB_DEBUG_UTILS_H
