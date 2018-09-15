#version 330

uniform vec3 colour;
uniform float ambient;
uniform float attenuation;
uniform float shininess;
uniform float strength;

in vec3 Normal;
in vec3 lightDirection;
in vec3 Half;
out vec4 fragColor;

void main() {
	float diffuse = ambient + max(0.0, dot(Normal, lightDirection)) * attenuation;
	float specular = max (0, dot(Normal, Half)) * strength;
	if (diffuse == 0.0) {
		specular = 0.0;
	} else {
		specular = pow(specular, shininess) * strength;
	}
	fragColor = vec4( colour * diffuse, 1 );
}
