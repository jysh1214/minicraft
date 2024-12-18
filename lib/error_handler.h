#ifndef MC_LIB_ERROR_HANDLER_H
#define MC_LIB_ERROR_HANDLER_H

#include <iostream>
#include <stdexcept>

inline void handleError(const std::string &errorMessage) {
  std::cerr << errorMessage << std::endl;
#ifdef ENABLE_EXCEPTION
  throw std::runtime_error(errorMessage);
#else
  exit(-1);
#endif
}

#endif // MC_LIB_ERROR_HANDLER_H
