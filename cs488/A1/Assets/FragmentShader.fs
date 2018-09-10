#version 330

uniform vec3 colour;
uniform vec3 lightDir;

in vec3 Normal;
out vec4 fragColor;

void main() {
	float diffuse = max(0.0, dot(Normal, lightDir));
	fragColor = vec4( colour * diffuse, 1 );
}
