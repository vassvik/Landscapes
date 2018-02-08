#version 330 core

in vec3 position;
in vec3 position2;
in vec3 normal;

uniform int hover_triangle;
uniform int clicked_triangle;

uniform float clicked_radius;

uniform vec3 hover_position;
uniform vec3 clicked_position;

layout(location = 0) out vec4 color;
layout(location = 1) out vec4 info;

void main() {
	vec3 click_color = vec3(0.2);
	vec3 hover_color = vec3(0.7, 1.0, 0.4);

	if (hover_triangle == gl_PrimitiveID) {
		color.xyz = hover_color;
	} else {
		color.xyz = 0.5 + 0.5*normal;
	}
	color.w = 1.0;

	if (clicked_triangle != -1) {
		float d = length(position2 - clicked_position);
		float w = 1.5*fwidth(d);
		float s = 0.5*smoothstep(w/2.0, -w/2.0, d - clicked_radius);
		color.xyz = color.xyz*(1.0 - s) + click_color*s;
	}


	info = vec4(float(gl_PrimitiveID), position2);
}