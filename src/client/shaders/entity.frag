#version 330 core

in vec3 frag_position;

out vec4 frag_colour;

uniform bool fullbright;
uniform vec3 colour;
uniform vec3 camera;

void main() {
	float dist = distance(frag_position, camera);
	float falloff = fullbright ? 1.0 : (1.0 / (1.0 + 0.05 * dist * dist));
	frag_colour = vec4(colour * falloff, 1.0);
}
