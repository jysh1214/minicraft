#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in float faceID;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

flat out float face_id;

void main() {
  gl_Position = projection * view * model * vec4(aPos, 1.0);
  face_id = faceID;
}
