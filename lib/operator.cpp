#include "operator.h"

#include <iostream>

namespace {

static bool firstMouse = true;
static float lastX = 0.0f;
static float lastY = 0.0f;

} // namespace

void keyboardInput(GLFWwindow *window, Camera &camera, const float deltaTime) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);

  const float cameraSpeed = camera.getSpeed() * deltaTime;
  const glm::vec3 cameraPos = camera.getPos();
  glm::vec3 newCameraPos = cameraPos;
  const glm::vec3 cameraFront = camera.getFront();
  const glm::vec3 cameraUp = camera.getUp();
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
    newCameraPos += cameraSpeed * cameraFront;
  }
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
    newCameraPos -= cameraSpeed * cameraFront;
  }
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
    newCameraPos -=
        glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
  }
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
    newCameraPos +=
        glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
  }
  camera.setPos(newCameraPos);
  camera.update();
}

void mouseInput(GLFWwindow *window, Camera *camera, const float xpos,
                const float ypos) {
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

  xoffset *= camera->getSensitivity();
  yoffset *= camera->getSensitivity();

  float yaw = camera->getYaw();
  float pitch = camera->getPitch();

  yaw += xoffset;
  pitch += yoffset;

  if (yaw > 360.f || yaw < -360.f)
    yaw = glm::mod(yaw, 360.f);

  if (pitch > 89.9f)
    pitch = 89.9f;
  if (pitch < -89.9f)
    pitch = -89.9f;

  camera->setYaw(yaw);
  camera->setPitch(pitch);
  camera->update();
}
