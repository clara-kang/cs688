#version 330

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
in vec3 VertexPosition;
in vec3 VertexNormal;

out vec3 Normal;

void main() {
	gl_Position = P * V * M * vec4(VertexPosition, 1.0);
	Normal = VertexNormal;
}
