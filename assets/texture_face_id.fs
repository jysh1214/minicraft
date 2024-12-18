#version 330 core

in vec2 texCoor;
out vec4 fragColor;

flat in float face_id;

uniform sampler2D textureAtlas;


const float layer = 1.0f / 2.0f; // two block type: air, grass

void main() {
  fragColor = texture(textureAtlas, vec2(face_id / 3.0f, layer * 1)).rgb;; // 1 is block type id
}
