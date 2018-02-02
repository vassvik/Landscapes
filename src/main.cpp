#include <stdio.h>

#include <glad/glad.h>
#include <glad/glad.c>
#include <GLFW/glfw3.h>

#include "utils.hpp"

int main() {
	//
	if (!glfwInit()) {
		printf("Error, could not initialize GLFW\n");
		return -1;
	}

	//
	glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//
	int resx = 1280, resy = 720;
	GLFWwindow *window = glfwCreateWindow(resx, resy, "Terrain Visualizer", NULL, NULL);

	//
	if (window == NULL) {
		printf("Error, could not create window.\n");
		glfwTerminate();
		return -2;
	}

	//
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // Enable vsync

	//
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		printf("Could not load OpenGL functions.\n");
		return -3;
	}

	// 
	GLuint program = load_shaders("src/vertex_shader.glsl", "src/fragment_shader.glsl");

	// 
	GLuint vao;
	glGenVertexArrays(1, &vao);

	//
	glClearColor(1.0, 0.7, 0.4 , 1.0);
	while (!glfwWindowShouldClose(window)) {
		//
		glfwPollEvents();

		//
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
			glfwSetWindowShouldClose(window, GLFW_TRUE);
		}

		//
		glClear(GL_COLOR_BUFFER_BIT);

		//
		glUseProgram(program);

		//
		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		//
		glfwSwapBuffers(window);
	}

	//
	glfwTerminate();

	//
	return 0;
}

