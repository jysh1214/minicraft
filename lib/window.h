#ifndef MC_LIB_WINDOW_H
#define MC_LIB_WINDOW_H

#include <functional>
#include <string>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "camera.h"
#include "error_handler.h"
#include "operator.h"

class Window {
public:
  Window(const std::string &name, const GLuint w, const GLuint h);
  ~Window();

  void update();
  bool isAlive();
  void close();
  void setCamera(Camera *camera);
  GLFWwindow *getGLWindow();

  void setFramebufferSizeCallback(
      std::function<void(GLFWwindow *, Camera *, const int, const int)>);
  void setCursorPosCallback(
      std::function<void(GLFWwindow *, Camera *, const float, const float)>);

private:
  Camera *camera = nullptr;
  GLFWwindow *window = nullptr;
  std::function<void(GLFWwindow *, Camera *, const int, const int)>
      framebufferSizeCallback;
  std::function<void(GLFWwindow *, Camera *, const float, const float)>
      cursorPosCallback;
};

void framebufferSizeCallback(GLFWwindow *window, Camera *camera, const int w,
                             const int h);

#endif // MC_LIB_WINDOW_H
