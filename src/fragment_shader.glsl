#version 330 core


layout(location = 0) out vec4 color;
layout(location = 1) out vec4 info;


in vec3 position;
in vec3 normal;


uniform int hover_triangle;
uniform vec3 hover_position;

uniform int clicked_triangle;
uniform vec3 clicked_position;
uniform float clicked_radius;


void main() {
	vec3 click_color = vec3(0.2);
	vec3 hover_color = vec3(0.7, 1.0, 0.4);

	// highlight hovered triangle
	if (hover_triangle == gl_PrimitiveID) {
		color.xyz = hover_color;
	} else {
		color.xyz = 0.5 + 0.5*normal;
	}
	color.w = 1.0;

	// highlight pixels inside sphere of influence if clicked
	if (clicked_triangle != -1) {
		float d = length(position - clicked_position);
		float w = 1.5*fwidth(d);
		float s = 0.5*smoothstep(w/2.0, -w/2.0, d - clicked_radius);
		color.xyz = color.xyz*(1.0 - s) + click_color*s;
	}

	// output which primitive is shown at this pixel
	info = vec4(float(gl_PrimitiveID), position);
}