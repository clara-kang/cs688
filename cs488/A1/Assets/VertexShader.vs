#version 330

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
uniform vec3 lightDir;
uniform vec3 EyePosition;
in vec3 VertexPosition;
in vec3 VertexNormal;
in vec2 in_tex_coord;

out vec3 Normal;
out vec3 lightDirection;
out vec3 Half;
out vec2 vs_tex_coord;

void main() {
	gl_Position = P * V * M * vec4(VertexPosition, 1.0);
	Normal = (M * vec4(VertexNormal, 0)).xyz;
	lightDirection = lightDir;
	Half = normalize(lightDir + EyePosition);
	vs_tex_coord = in_tex_coord;
}
