#version 330 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in int vertexVisibility;


uniform mat4 MVP;
uniform mat4 M;

out vec3 position;
out vec3 position2;
out vec3 normal;
flat out int visibility;

void main() {
	normal = vertexNormal;                         // model normals
	position = vertexPosition;                     // model positions
	position2 = (M*vec4(vertexPosition, 1.0)).xyz; // world positions
	visibility = vertexVisibility;
	
	gl_Position = MVP*vec4(vertexPosition, 1.0);
}