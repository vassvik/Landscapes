#version 330 core


out vec3 position;
out vec3 normal;


layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;


uniform mat4 MVP;
uniform mat4 M;

void main() {
	normal = vertexNormal;                        // model normals
	position = (M*vec4(vertexPosition, 1.0)).xyz; // world positions
	
	gl_Position = MVP*vec4(vertexPosition, 1.0);
}