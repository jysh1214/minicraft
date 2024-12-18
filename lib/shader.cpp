#include "shader.h"

namespace {

inline std::tuple<fs::path, fs::path> getShaderPath(const VertexFormat format) {
  fs::path vs = fs::path(SHADER_SOURCE_DIR);
  fs::path fs = fs::path(SHADER_SOURCE_DIR);

  switch (format) {
  case VertexFormat::PosColor:
    vs = vs / "color.vs";
    fs = fs / "color.fs";
    break;
  case VertexFormat::PosTexCoor:
    vs = vs / "texture.vs";
    fs = fs / "texture.fs";
    break;
  default:
    handleError("No such type shader.");
  }

  return std::make_tuple(vs, fs);
}

inline const std::string readShaderFromFile(const std::string &filePath) {
  std::ifstream shaderFile;
  std::stringstream shaderStream;
  // Ensure ifstream can throw exceptions
  shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  try {
    shaderFile.open(filePath);
    // read file's buffer contents into streams
    shaderStream << shaderFile.rdbuf();
    // close file handlers
    shaderFile.close();
    // Convert stream into string
    return shaderStream.str();
  } catch (std::ifstream::failure e) {
    handleError("ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " +
                std::string(e.what()));
  }

  return "";
}

inline void checkCompileError(const GLuint shader, const std::string &type) {
  GLint success;
  GLchar infoLog[1024];
  auto check = [&](const GLuint shader, const bool success) {
    if (success)
      return;
    std::string err = (type != "PROGRAM") ? "SHADER_COMPILATION_ERROR"
                                          : "PROGRAM_LINKING_ERROR";
    glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
    std::string errorMessage =
        "Check Compile Error: " + err + " of type: " + type + infoLog;
    handleError(errorMessage);
  };
  if (type != "PROGRAM") {
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  } else {
    glGetProgramiv(shader, GL_LINK_STATUS, &success);
  }
  check(shader, success);

  return;
}

} // namespace

Shader::Shader(const VertexFormat format) {

  auto [vertexShaderPath, fragmentShaderPath] = ::getShaderPath(format);

  // vertex shader
  const std::string vertexShaderSource =
      ::readShaderFromFile(vertexShaderPath.string());
  const GLchar *vertexShaderSourcePtr =
      static_cast<const GLchar *>(vertexShaderSource.c_str());

  GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexShaderSourcePtr, nullptr);
  glCompileShader(vertexShader);
  ::checkCompileError(vertexShader, "VERTEX");

  // fragment Shader
  const std::string fragmentShaderSource =
      ::readShaderFromFile(fragmentShaderPath.string());
  const GLchar *fragmentShaderSourcePtr =
      static_cast<const GLchar *>(fragmentShaderSource.c_str());

  GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentShaderSourcePtr, NULL);
  glCompileShader(fragmentShader);
  ::checkCompileError(fragmentShader, "FRAGMENT");

  // shader Program
  this->ID = glCreateProgram();
  glAttachShader(this->ID, vertexShader);
  glAttachShader(this->ID, fragmentShader);
  glLinkProgram(this->ID);
  ::checkCompileError(ID, "PROGRAM");

  // delete the shaders as they're linked into our program now and no longer
  // necessary
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);
}

Shader::~Shader() { glDeleteProgram(this->ID); }

GLuint Shader::getID() { return this->ID; }

void Shader::use() { glUseProgram(this->ID); }

void Shader::destroy() { this->~Shader(); }

void Shader::setInt(const std::string &name, int value) {
  glUniform1i(glGetUniformLocation(this->ID, name.c_str()), value);
}

void Shader::setMat4(const std::string &name, const glm::mat4 &mat) {
  glUniformMatrix4fv(glGetUniformLocation(this->ID, name.c_str()), 1, GL_FALSE,
                     &mat[0][0]);
}
