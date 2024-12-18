#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#undef GLM_ENABLE_EXPERIMENTAL

#include "block.h"
#include "block_type.h"
#include "operator.h"
#include "render.h"
#include "shader.h"
#include "window.h"

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// timing
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;
GLfloat lastFpsUpdate = 0.0f;

int main() {
  // Create camera
  Camera camera;
  camera.setSpeed(2.5f);
  camera.setScreenSize((float)SCR_WIDTH, (float)SCR_HEIGHT);
  camera.setPos(glm::vec3(0.0f, 0.0f, 3.0f));
  camera.setFront(glm::vec3(0.0f, 0.0f, -1.0f));
  camera.setUp(glm::vec3(0.0f, 1.0f, 0.0f));
  camera.setYaw(-90.0f);
  camera.setPitch(0.0f);
  camera.setFovy(glm::radians(45.0f));
  camera.setNear(0.1f);
  camera.setFar(100.0f);
  camera.update();

  // Create window
  Window window("minicraft", (float)SCR_WIDTH, (float)SCR_HEIGHT);
  // Register callback function
  window.setCamera(&camera);
  window.setCursorPosCallback(mouseInput);
  window.setFramebufferSizeCallback(framebufferSizeCallback);

  // Create shader
  fs::path vertexShaderPath = fs::path(SHADER_SOURCE_DIR) /
                              "vertex_shader.glsl"; // TODO: Use shader factory
  fs::path fragmentShaderPath =
      fs::path(SHADER_SOURCE_DIR) /
      "fragment_shader.glsl"; // TODO: Use shader factory
  Shader shader(vertexShaderPath, fragmentShaderPath);

  // Create a block
  Block<VertexFormat::Color> block(BlockType::Grass);

  // Render
  Render<VertexFormat::Color> render(block);
  render.bind();

  shader.use();

  // pass projection matrix to shader
  const glm::mat4 projection = camera.getProjection();
  shader.setMat4("projection", projection);

  //
  GLuint frameCount = 0;

  // Render loop
  while (window.isAlive()) {

    // per frame
    GLfloat currentFrame = static_cast<GLfloat>(glfwGetTime());
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    keyboardInput(window.getGLWindow(), camera, deltaTime);

    glClearColor(0.53f, 0.81f, 0.92f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // use our shader
    // shader.use();

    // set view
    const glm::mat4 view = camera.getView();
    shader.setMat4("view", view);

    // Only draw lines
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-1.0f, -1.0f, 0.0f));
    model =
        glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.5f, 1.0f, 0.0f));
    shader.setMat4("model", model);
    // glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    render.draw();

    // Check and call events and swap the buffers
    window.update();

    // FPS calculation and display
    frameCount++;
    if (currentFrame - lastFpsUpdate >= 1.0f) {
      std::cout << "FPS: " << frameCount << std::endl;
      frameCount = 0;
      lastFpsUpdate = currentFrame;
    }
  }
  window.close();

  return 0;
}
