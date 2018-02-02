#version 330 core

in vec3 position;
in vec3 normal;

uniform int chosen_triangle;

layout(location = 0) out vec4 color;
layout(location = 1) out vec4 info;

void main() {
	if (gl_PrimitiveID == chosen_triangle)
		color = vec4(0.5 + 0.5*normal, 1.0);
	else
		color = vec4(0.5 - 0.5*normal, 1.0);

	info = vec4(float(1.0+gl_PrimitiveID), 0.0, 0.0, 0.0);
}