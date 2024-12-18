#ifndef MC_LIB_CAMERA_H
#define MC_LIB_CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
  Camera(const float width, const float height);
  ~Camera() = default;

  const glm::mat4 getView() const;
  const glm::mat4 getProjection() const;

  const float getSpeed() const;
  const float getSensitivity() const;
  const glm::vec3 getPos() const;
  const glm::vec3 getFront() const;
  const glm::vec3 getUp() const;
  const glm::vec3 getRight() const;

  const float getYaw() const;
  const float getPitch() const;

  const float getFovy() const;
  const float getFovx() const;
  const float getAspect() const;
  const float getNear() const;
  const float getFar() const;

  void setSpeed(const float speed);
  void setSensitivity(const float sensitivity);
  void setScreenSize(const float width, const float height);

  void setPos(const glm::vec3 &pos);
  void setFront(const glm::vec3 &front);
  void setUp(const glm::vec3 &up);
  void setYaw(const float yaw);
  void setPitch(const float pitch);

  void setFovy(const float fovy);
  void setNear(const float near);
  void setFar(const float far);

  void update();

private:
  float width;
  float height;
  float speed;
  float sensitivity;

  glm::mat4 view;
  glm::mat4 projection;

  glm::vec3 pos;
  glm::vec3 front;
  glm::vec3 up;
  glm::vec3 right;

  float yaw;
  float pitch;

  float fovy;
  float fovx;
  float aspect;
  float near;
  float far;
};

#endif // MC_LIB_CAMERA_H
