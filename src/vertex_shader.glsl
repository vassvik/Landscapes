#version 330 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;

uniform float time;
uniform int draw_mode;

out vec3 position;
out vec3 normal;

void main() {
	if (draw_mode == 0) {
		position = vertexPosition;
		normal = vertexNormal;

		vec3 p = vertexPosition;
		p.xy = 2.0*p.xy - 1.0;
		p *= 0.5;

		float f = 0.1;

		p.xy = vec2(p.x*cos(f*time) - p.y*sin(f*time), p.x*sin(f*time) + p.y*cos(f*time));
		p.yz = vec2(p.y*cos(120.0*3.1415/180.0) + p.z*sin(120.0*3.1415/180.0), -p.y*sin(120.0*3.1415/180.0) + p.z*cos(120.0*3.1415/180.0));

		gl_Position = vec4(p, 1.0);
	} else {
		vec2 p = vec2(gl_VertexID%2, gl_VertexID/2);
		position = vec3((2.0 - cos(0.0*time))*p, 0.0);

		gl_Position = vec4(2.0*p-1.0, 0.0, 1.0);
	}
}