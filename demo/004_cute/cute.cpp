#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "block.h"
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
  Camera camera((float)SCR_WIDTH, (float)SCR_HEIGHT);
  camera.setSpeed(2.5f);
  camera.setPos(glm::vec3(0.0f, 10.0f, 0.0f));
  camera.setFront(glm::vec3(0.0f, 0.0f, 0.0f));
  camera.setUp(glm::vec3(0.0f, 1.0f, 0.0f));
  camera.setYaw(-90.0f);
  camera.setPitch(0.0f);
  camera.setFovy(glm::radians(45.0f));
  camera.setNear(0.1f);
  camera.setFar(500.0f); // 100.0f
  camera.update();

  // Create window
  Window window("minicraft", (float)SCR_WIDTH, (float)SCR_HEIGHT);
  // Register callback function
  window.setCamera(&camera);
  window.setCursorPosCallback(mouseInput);
  window.setFramebufferSizeCallback(framebufferSizeCallback);

  // Create shader
  Shader shader(VertexFormat::PosTexCoor);
  shader.use();
  shader.setInt("texture1", 0);

  // Set chunk
  const GLuint x = 32, z = 32, y = 32;
  Chunk<VertexFormat::PosTexCoor>::setSize({x, y, z});

  // Create a world
  World<VertexFormat::PosTexCoor> world;
  world.setSize({10, 2, 10});
  world.init();

  // Render
  Render<VertexFormat::PosTexCoor> render;

  // pass projection matrix to shader
  const glm::mat4 projection = camera.getProjection();
  shader.setMat4("projection", projection);

  // FPS
  GLuint frameCount = 0;

  while (window.isAlive()) {

    // per frame
    GLfloat currentFrame = static_cast<GLfloat>(glfwGetTime());
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    keyboardInput(window.getGLWindow(), camera, deltaTime);

    glClearColor(0.53f, 0.81f, 0.92f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // set view
    const glm::mat4 view = camera.getView();
    shader.setMat4("view", view);

    // Only draw lines
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glm::mat4 model = glm::mat4(1.0f);
    shader.setMat4("model", model);

    render.draw(camera, world);

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
