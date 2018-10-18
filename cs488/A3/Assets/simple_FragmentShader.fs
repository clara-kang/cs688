#version 330

in vec3 pix_color;

out vec4 fragColour;

void main() {
	fragColour = vec4(pix_color,1.0);
}
