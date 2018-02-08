#version 330 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;

uniform float time;

out vec3 position;
out vec3 position2;
out vec3 normal;

void main() {
	position = vertexPosition;
	normal = vertexNormal;

	vec3 p = vertexPosition;
	p.xy = 2.0*(p.xy - vec2(1.0, 0.5));
	p *= 0.5;

	position2 = p;
	float f = 0.1;
	p.xy = vec2(p.x*cos(f*time) - p.y*sin(f*time), p.x*sin(f*time) + p.y*cos(f*time));
	p.yz = vec2(p.y*cos(120.0*3.1415/180.0) + p.z*sin(120.0*3.1415/180.0), -p.y*sin(120.0*3.1415/180.0) + p.z*cos(120.0*3.1415/180.0));


	gl_Position = vec4(p, 1.0);
}