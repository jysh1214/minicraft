#ifndef MC_LIB_SHADER_H
#define MC_LIB_SHADER_H

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <tuple>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "error_handler.h"
#include "vertex_format.h"

namespace fs = std::filesystem;

class Shader {
public:
  Shader(const VertexFormat format);
  ~Shader();
  GLuint getID();
  void use();
  void destroy();

  void setInt(const std::string &name, int value);
  void setMat4(const std::string &name, const glm::mat4 &mat);

private:
  GLuint ID;
};

#endif // MC_LIB_SHADER_H
