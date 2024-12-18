#include "window.h"

namespace {

// ref:
// https://stackoverflow.com/questions/28746744/passing-capturing-lambda-as-function-pointer
template <typename Function> struct function_traits;

template <typename Ret, typename... Args> struct function_traits<Ret(Args...)> {
  typedef Ret (*ptr)(Args...);
};

template <typename Ret, typename... Args>
struct function_traits<Ret (*const)(Args...)> : function_traits<Ret(Args...)> {
};

template <typename Cls, typename Ret, typename... Args>
struct function_traits<Ret (Cls::*)(Args...) const>
    : function_traits<Ret(Args...)> {};

using voidfun = void (*)();

template <typename F> voidfun lambda_to_void_function(F lambda) {
  static auto lambda_copy = lambda;
  return []() { lambda_copy(); };
}

// requires C++20
template <typename F>
auto lambda_to_pointer(F lambda) ->
    typename function_traits<decltype(&F::operator())>::ptr {
  static auto lambda_copy = lambda;
  return []<typename... Args>(Args... args) { return lambda_copy(args...); };
}

} // namespace

Window::Window(const std::string &name, const GLuint w, const GLuint h) {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  this->window = glfwCreateWindow(w, h, name.c_str(), nullptr, nullptr);
  if (this->window == nullptr) {
    glfwTerminate();
    handleError("Failed to create GLFW window.");
  }

  glfwMakeContextCurrent(window);

  if (glewInit() != GLEW_OK) {
    handleError("Failed to initialize GLEW.");
  }

  glEnable(GL_DEPTH_TEST);

  // tell GLFW to capture our mouse
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

Window::~Window() { glfwTerminate(); }

void Window::update() {
  // Check and call events and swap the buffers
  glfwPollEvents();
  glfwSwapBuffers(this->window);
}

bool Window::isAlive() { return !glfwWindowShouldClose(this->window); }

void Window::close() { glfwSetWindowShouldClose(this->window, true); }

void Window::setCamera(Camera *camera) { this->camera = camera; }

GLFWwindow *Window::getGLWindow() { return this->window; }

void Window::setFramebufferSizeCallback(
    const std::function<void(GLFWwindow *, Camera *, const int, const int)>
        callback) {
  this->framebufferSizeCallback = callback;
  glfwSetFramebufferSizeCallback(
      this->window,
      lambda_to_pointer([&](GLFWwindow *window, int w, int h) -> void {
        this->framebufferSizeCallback(this->window, this->camera, w, h);
      }));
}

void Window::setCursorPosCallback(
    const std::function<void(GLFWwindow *, Camera *, const float, const float)>
        callback) {
  this->cursorPosCallback = callback;
  glfwSetCursorPosCallback(
      this->window, lambda_to_pointer([&](GLFWwindow *window, double xPos,
                                          double yPos) -> void {
        this->cursorPosCallback(this->window, this->camera,
                                static_cast<float>(xPos),
                                static_cast<float>(yPos));
      }));
}

void framebufferSizeCallback(GLFWwindow *window, Camera *camera, const int w,
                             const int h) {
  glViewport(0, 0, w, h);
  camera->setScreenSize(w, h);
  camera->update();
}
