#version 330

in vec3 position;

uniform mat4 ModelView;
uniform mat4 Perspective;
uniform vec3 obj_color;

out vec3 pix_color;

void main() {
  pix_color = obj_color;
  gl_Position = Perspective * ModelView * vec4(position, 1.0);
}
