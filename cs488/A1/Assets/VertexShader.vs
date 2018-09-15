#version 330

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
uniform vec3 lightDir;
uniform vec3 EyePosition;
in vec3 VertexPosition;
in vec3 VertexNormal;

out vec3 Normal;
out vec3 lightDirection;
out vec3 Half;

void main() {
	gl_Position = P * V * M * vec4(VertexPosition, 1.0);
	Normal = VertexNormal;
	lightDirection = lightDir;
	Half = normalize(lightDir + EyePosition);
}
