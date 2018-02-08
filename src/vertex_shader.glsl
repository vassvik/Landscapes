#version 330 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;

uniform float time;
uniform mat4 MVP;
uniform mat4 M;

out vec3 position;
out vec3 position2;
out vec3 normal;

void main() {
	position = vertexPosition;
	normal = vertexNormal;
	position2 = (M*vec4(vertexPosition, 1.0)).xyz;

	gl_Position = MVP*vec4(vertexPosition, 1.0);
}