#include "camera.h"

Camera::Camera(const float width, const float height)
    // clang-format off
    : width{width}
    , height{height}
    , speed{1.0f}
    , sensitivity{0.1f}
    , pos{glm::vec3(1.0f)}
    , front{glm::vec3(1.0f)}
    , up{glm::vec3(0.0f, 1.0f, 0.0f)}
    , yaw{0.1f}
    , pitch{0.1}
    , fovy{glm::radians(45.0f)}
    , near{0.1f}
    , far{100.0f} {
  // clang-format on
  this->aspect = this->width / this->height;
  this->fovx = 2.0f * std::atan(std::tan(this->fovy * 0.5f) * this->aspect);
  this->update();
}

const glm::mat4 Camera::getView() const { return this->view; }

const glm::mat4 Camera::getProjection() const { return this->projection; }

const float Camera::getSpeed() const { return this->speed; }

const float Camera::getSensitivity() const { return this->sensitivity; }

const glm::vec3 Camera::getPos() const { return this->pos; }

const glm::vec3 Camera::getFront() const { return this->front; }

const glm::vec3 Camera::getUp() const { return this->up; }

const glm::vec3 Camera::getRight() const { return this->right; }

const float Camera::getYaw() const { return this->yaw; }

const float Camera::getPitch() const { return this->pitch; }

const float Camera::getFovy() const { return this->fovy; }

const float Camera::getFovx() const { return this->fovx; }

const float Camera::getAspect() const { return this->aspect; }

const float Camera::getNear() const { return this->near; }

const float Camera::getFar() const { return this->far; }

void Camera::setSpeed(const float speed) { this->speed = speed; }

void Camera::setSensitivity(const float sensitivity) {
  this->sensitivity = sensitivity;
}

void Camera::setScreenSize(const float width, const float height) {
  this->width = width;
  this->height = height;
  this->aspect = this->width / this->height;
  this->fovx = 2.0f * std::atan(std::tan(this->fovy * 0.5f) * this->aspect);
}

void Camera::setPos(const glm::vec3 &pos) { this->pos = pos; }

void Camera::setFront(const glm::vec3 &front) { this->front = front; }

void Camera::setUp(const glm::vec3 &up) { this->up = up; }

void Camera::setYaw(const float yaw) { this->yaw = yaw; }

void Camera::setPitch(const float pitch) { this->pitch = pitch; }

void Camera::setFovy(const float fovy) {
  this->fovy = fovy;
  this->fovx = 2.0f * std::atan(std::tan(this->fovy * 0.5f) * this->aspect);
}

void Camera::setNear(const float near) { this->near = near; }

void Camera::setFar(const float far) { this->far = far; }

void Camera::update() {
  // Update front vector
  glm::vec3 cameraFront;
  cameraFront.x = cos(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
  cameraFront.y = sin(glm::radians(this->pitch));
  cameraFront.z = sin(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
  this->front = glm::normalize(cameraFront);
  // Update vire matrix
  this->right = glm::normalize(glm::cross(cameraFront, this->up));
  this->view = glm::lookAt(this->pos, this->pos + this->front, this->up);
  // Update the projection matrix
  this->projection =
      glm::perspective(this->fovy, this->aspect, this->near, this->far);
}
