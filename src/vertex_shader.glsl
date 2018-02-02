#version 430 core

void main() {
	vec2 p = 2.0*vec2(gl_VertexID % 2, gl_VertexID / 2) - 1.0;
	gl_Position = vec4(0.5*p, 0.0, 1.0);
}