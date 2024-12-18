#ifndef MC_LIB_OPERATOR_H
#define MC_LIB_OPERATOR_H

#include <GLFW/glfw3.h>

#include "camera.h"

void keyboardInput(GLFWwindow *window, Camera &camera, const float deltaTime);
void mouseInput(GLFWwindow *window, Camera *camera, const float xPosIn,
                const float yPosIn);

#endif // MC_LIB_OPERATOR_H
