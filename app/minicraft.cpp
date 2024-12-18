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

#include "mesh_buffer.h"
#include "obj.h"
#include "shader.h"

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

bool firstMouse = true;
float yaw = -90.0f;
float pitch = 0.0f;
float lastX = 800.0f / 2.0f;
float lastY = 600.0f / 2.0f;
float fov = 45.0f;

// world space positions of our cubes
glm::vec3 cubePositions[] = {
    glm::vec3(0.0f, 0.0f, 0.0f),    glm::vec3(2.0f, 5.0f, -15.0f),
    glm::vec3(-1.5f, -2.2f, -2.5f), glm::vec3(-3.8f, -2.0f, -12.3f),
    glm::vec3(2.4f, -0.4f, -3.5f),  glm::vec3(-1.7f, 3.0f, -7.5f),
    glm::vec3(1.3f, -2.0f, -2.5f),  glm::vec3(1.5f, 2.0f, -2.5f),
    glm::vec3(1.5f, 0.2f, -1.5f),   glm::vec3(-1.3f, 1.0f, -1.5f)};

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// process all input: query GLFW whether relevant keys are pressed/released this
// frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);

  float cameraSpeed = static_cast<float>(2.5 * deltaTime);
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    cameraPos += cameraSpeed * cameraFront;
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    cameraPos -= cameraSpeed * cameraFront;
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    cameraPos -=
        glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    cameraPos +=
        glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

// glfw: whenever the window size changed (by OS or user resize) this callback
// function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  // make sure the viewport matches the new window dimensions; note that width
  // and height will be significantly larger than specified on retina displays.
  glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow *window, double xposIn, double yposIn) {
  float xpos = static_cast<float>(xposIn);
  float ypos = static_cast<float>(yposIn);

  if (firstMouse) {
    lastX = xpos;
    lastY = ypos;
    firstMouse = false;
  }

  float xoffset = xpos - lastX;
  float yoffset =
      lastY - ypos; // reversed since y-coordinates go from bottom to top
  lastX = xpos;
  lastY = ypos;

  float sensitivity = 0.1f; // change this value to your liking
  xoffset *= sensitivity;
  yoffset *= sensitivity;

  yaw += xoffset;
  pitch += yoffset;

  // make sure that when pitch is out of bounds, screen doesn't get flipped
  if (pitch > 89.0f)
    pitch = 89.0f;
  if (pitch < -89.0f)
    pitch = -89.0f;

  glm::vec3 front;
  front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
  front.y = sin(glm::radians(pitch));
  front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
  cameraFront = glm::normalize(front);
}

int main() {
  // Init GLFW, GLEW
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow *window =
      glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "minicraft", nullptr, nullptr);
  if (window == nullptr) {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetCursorPosCallback(window, mouse_callback);

  // tell GLFW to capture our mouse
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  if (glewInit() != GLEW_OK) {
    std::cout << "Failed to initialize GLEW" << std::endl;
    return -1;
  }

  // Create shader
  fs::path vertexShaderPath = fs::path(SHADER_SOURCE_DIR) /
                              "vertex_shader.glsl"; // TODO: Use shader factory
  fs::path fragmentShaderPath =
      fs::path(SHADER_SOURCE_DIR) /
      "fragment_shader.glsl"; // TODO: Use shader factory
  Shader shader(vertexShaderPath, fragmentShaderPath);

  glEnable(GL_DEPTH_TEST);

  // // Create a rectangle
  // Obj<ObjType::Cube, VertexAttr::Color> cube;

  // // mesh buffer
  // MeshBuffer<ObjType::Cube, VertexAttr::Color> mesh(cube);

  // MeshBuffer<ObjType::Cube, VertexAttr::Color> activateMesh(cube);

  // // pass projection matrix to shader
  // shader.use();
  // glm::mat4 projection = glm::mat4(1.0f);
  // projection = glm::perspective(
  //     glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f,
  //     100.0f);
  // shader.setMat4("projection", projection);

  // // Render loop
  // while (!glfwWindowShouldClose(window)) {

  //   // per frame
  //   GLfloat currentFrame = static_cast<GLfloat>(glfwGetTime());
  //   deltaTime = currentFrame - lastFrame;
  //   lastFrame = currentFrame;

  //   processInput(window);

  //   // Rendering commands here
  //   glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
  //   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  //   // use our shader
  //   shader.use();

  //   // set view
  //   glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront,
  //   cameraUp); shader.setMat4("view", view);

  //   // render chunk
  //   mesh.bind();
  //   for (unsigned int i = 0; i < 10; i++) {
  //     // calculate the model matrix for each object and pass it to shader
  //     before
  //     // drawing
  //     glm::mat4 model = glm::mat4(1.0f);
  //     model = glm::translate(model, cubePositions[i]);
  //     float angle = 20.0f * i;
  //     model =
  //         glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f,
  //         0.5f));
  //     shader.setMat4("model", model);
  //     glDrawElements(GL_TRIANGLES, 36 /*TODO use better way*/,
  //     GL_UNSIGNED_INT,
  //                    0);
  //   }

  //   // render activated block
  //   {
  //     activateMesh.bind();
  //     glm::mat4 model = glm::mat4(1.0f);
  //     model = glm::translate(model, glm::vec3(-1.0f, -1.0f, 0.0f));
  //     model =
  //         glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.5f, 1.0f,
  //         0.0f));

  //     shader.setMat4("model", model);

  //     // render box
  //     glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
  //   }

  //   // Only draw lines
  //   // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  //   // Check and call events and swap the buffers
  //   glfwPollEvents();
  //   glfwSwapBuffers(window);
  // }
  // // MeshBuffer<ObjType::Cube, VertexAttr::Color>::unbind();

  // glfwTerminate();

  return 0;
}
