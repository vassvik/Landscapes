#version 330 core

in vec3 position;
in vec3 position2;
in vec3 normal;
flat in int visibility;

uniform int hover_triangle;
uniform int clicked_triangle;

uniform float clicked_radius;

uniform vec3 hover_position;
uniform vec3 clicked_position;

uniform float time;

layout(location = 0) out vec4 color;
layout(location = 1) out vec4 info;

uniform bool wireframe;

vec3 palette( float t, vec3 a, vec3 b, vec3 c, vec3 d ) {
    return a + b*cos( 6.28318*(c*t+d) );
}

void main() {
	vec3 click_color = vec3(0.2);
	vec3 hover_color = vec3(0.7, 1.0, 0.4);

	if (hover_triangle == gl_PrimitiveID) {
		color.xyz = hover_color;
	} else {
		color.xyz = 0.5 + 0.5*normal;
		//color.xyz = palette( visibility/10.0, vec3(0.5,0.5,0.5),vec3(0.5,0.5,0.5),vec3(1.0,1.0,1.0),vec3(0.0,0.33,0.67) );
	    //color.xyz = palette( visibility/10.0, vec3(0.5,0.5,0.5),vec3(0.5,0.5,0.5),vec3(1.0,1.0,1.0),vec3(0.0,0.10,0.20) );
	    //color.xyz = palette( visibility/10.0, vec3(0.5,0.5,0.5),vec3(0.5,0.5,0.5),vec3(1.0,1.0,1.0),vec3(0.3,0.20,0.20) );
	    //color.xyz = palette( visibility/10.0, vec3(0.5,0.5,0.5),vec3(0.5,0.5,0.5),vec3(1.0,1.0,0.5),vec3(0.8,0.90,0.30) );
	    //color.xyz = palette( visibility/10.0, vec3(0.5,0.5,0.5),vec3(0.5,0.5,0.5),vec3(1.0,0.7,0.4),vec3(0.0,0.15,0.20) );
	    //color.xyz = palette( visibility/10.0, vec3(0.5,0.5,0.5),vec3(0.5,0.5,0.5),vec3(2.0,1.0,0.0),vec3(0.5,0.20,0.25) );
	    //color.xyz = palette( visibility/10.0, vec3(0.8,0.5,0.4),vec3(0.2,0.4,0.2),vec3(2.0,1.0,1.0),vec3(0.0,0.25,0.25) );
	}
	color.w = 1.0;

	if (clicked_triangle != -1) {
		float d = length(position2 - clicked_position);
		float w = 1.5*fwidth(d);
		float s = 0.5*smoothstep(w/2.0, -w/2.0, d - clicked_radius);
		color.xyz = color.xyz*(1.0 - s) + click_color*s;
	}


	info = vec4(float(gl_PrimitiveID), position2);

	//if (wireframe) color.xyz = vec3(0.0);
}