#include <stdio.h>

#include <glad/glad.h>
#include <glad/glad.c>
#include <GLFW/glfw3.h>

#include "utils.hpp"
#include "imgui_impl_glfw_gl3.cpp"


int main() {
	//
	if (!glfwInit()) {
		printf("Error, could not initialize GLFW\n");
		return -1;
	}

	//
	//glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//
	int resx = 1000, resy = 1000;
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

	ImGui::CreateContext();
	ImGui_ImplGlfwGL3_Init(window, true);
	//ImGui::StyleColorsDark();

	// 
	GLuint program = load_shaders("src/vertex_shader.glsl", "src/fragment_shader.glsl");

	//
	Mesh mesh;
	int Nx = 20;
	int Ny = 10;

	int num_vertices = (Nx+1)*(Ny+1);
	Vec3 *base_vertices = new Vec3[3*num_vertices];

	float dx = 1.0/float(Nx)*float(Nx)/float(Ny);
	float dy = 1.0/float(Ny);
	for (int j = 0; j <= Ny; j++) {
		for (int i = 0; i <= Nx; i++) {
			int k = j*(Nx+1) + i;
			base_vertices[k] = Vec3{float(i)*dx, float(j)*dy, 0.5f*cosf(2.0*3.14*i*dx)*cosf(2.0*3.14*j*dy)};
		}
	}

	mesh.num_vertices = Nx*Ny*6;
	mesh.vertices = new Vertex[mesh.num_vertices];

	for (int j = 0; j < Ny; j++) {
		for (int i = 0; i < Nx; i++) {
			int k = j*Nx + i;

			int i0 = i + 0;
			int i1 = i + 1;
			int j0 = j + 0;
			int j1 = j + 1;

			int k00 = j0*(Nx+1) + i0;
			int k10 = j0*(Nx+1) + i1;
			int k01 = j1*(Nx+1) + i0;
			int k11 = j1*(Nx+1) + i1;

			Vec3 v00 = base_vertices[k00];
			Vec3 v10 = base_vertices[k10];
			Vec3 v01 = base_vertices[k01];
			Vec3 v11 = base_vertices[k11];

			Vec3 n0 = normalize(cross(v10 - v00, v11 - v00));
			Vec3 n1 = normalize(cross(v11 - v00, v01 - v00));

			mesh.vertices[6*k+0] = Vertex{v00, n0};
			mesh.vertices[6*k+1] = Vertex{v10, n0};
			mesh.vertices[6*k+2] = Vertex{v11, n0};
			mesh.vertices[6*k+3] = Vertex{v00, n1};
			mesh.vertices[6*k+4] = Vertex{v11, n1};
			mesh.vertices[6*k+5] = Vertex{v01, n1};
		}
	}


	// 
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint vbo;
	glGenBuffers(1, &vbo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*mesh.num_vertices, &mesh.vertices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(Vec3)*0));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(Vec3)*1)); 

    glEnable(GL_DEPTH_TEST);



    //  Setup framebuffers
    
    GLuint screen_texture;
    glGenTextures(1, &screen_texture);
    glBindTexture(GL_TEXTURE_2D, screen_texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, resx, resy, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    GLuint pick_texture;
    glGenTextures(1, &pick_texture);
    glBindTexture(GL_TEXTURE_2D, pick_texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, resx, resy, 0, GL_RGBA, GL_FLOAT, nullptr);


    GLuint framebuffer;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screen_texture, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, pick_texture, 0);

    GLuint buffers[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, &buffers[0]);


    GLuint depthrenderbuffer;
    glGenRenderbuffers(1, &depthrenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, resx, resy);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);

    int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) printf("ERR framebuffers\n"); 

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

	int hover_triangle = -1;
    Vec3 hover_position;

    int clicked_triangle = -1;
    Vec3 clicked_position;

    float clicked_radius = 0.1;

	//
	glClearColor(1.0, 0.7, 0.4 , 1.0);
	while (!glfwWindowShouldClose(window)) {
		// let GLFW handle OS events and update internal input data
		glfwPollEvents();

		// close the window at the end of the frame if Escape is pressed
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
			glfwSetWindowShouldClose(window, GLFW_TRUE);
		}

        ImGui_ImplGlfwGL3_NewFrame();

		//
		glUseProgram(program);
		glUniform1f(glGetUniformLocation(program, "time"), glfwGetTime());

		// render to framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//
		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, mesh.num_vertices);

		//
		glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer);
        glReadBuffer(GL_COLOR_ATTACHMENT1);

        //
        double mx, my;
        glfwGetCursorPos(window, &mx, &my);

        float data[4];
        glReadPixels(mx, resy-my-1, 1, 1, GL_RGBA, GL_FLOAT, &data[0]);

        // update the uniform to tell the shader which primitive is hovered
        ImGui::Text("data = %f %f %f %f", data[0], data[1], data[2], data[3]);
        
        bool hit_terrain = !((fabs(data[0] - 1.0) < 1.0e-4 && fabs(data[1] - 0.7) < 1.0e-4 && fabs(data[2] - 0.4) < 1.0e-4));

        if (hit_terrain) {
        	hover_triangle = int(data[0] + 0.5);
        	hover_position = Vec3{data[1], data[2], data[3]};
        } else {
        	hover_triangle = -1;
        	hover_position = Vec3{0.0, 0.0, 0.0};
        }

        if (hover_triangle != -1) {
        	ImGui::Text("Hover: %d at {%f %f %f}", hover_triangle, hover_position.x, hover_position.y, hover_position.z);
        } else {
        	ImGui::Text("Hover: None");
        }
        

        if (ImGui::IsMouseClicked(0)) {
        	clicked_triangle = int(data[0] + 0.5);
        	clicked_position = Vec3{data[1], data[2], data[3]};
        } else if (ImGui::IsMouseReleased(0)) {
        	clicked_triangle = int(-1);
        	clicked_position = Vec3{0.0, 0.0, 0.0};
        }

        if (clicked_triangle != -1) {
        	ImGui::Text("Clicked: %d at {%f %f %f}", clicked_triangle, clicked_position.x, clicked_position.y, clicked_position.z);
        } else {
        	ImGui::Text("Clicked: None");
        }

        clicked_radius += ImGui::GetIO().MouseWheel*0.01;
        if (clicked_radius < 0.0) clicked_radius = 0.0;
        if (clicked_radius > 0.5) clicked_radius = 0.5;


    	glUniform1i(glGetUniformLocation(program, "hover_triangle"), hover_triangle);
    	glUniform1i(glGetUniformLocation(program, "clicked_triangle"), clicked_triangle);

    	glUniform3f(glGetUniformLocation(program, "hover_position"), hover_position.x, hover_position.y, hover_position.z);
    	glUniform3f(glGetUniformLocation(program, "clicked_position"), clicked_position.x, clicked_position.y, clicked_position.z);

    	glUniform1f(glGetUniformLocation(program, "clicked_radius"), clicked_radius);

        //
        glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer);
        glReadBuffer(GL_COLOR_ATTACHMENT0);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glBlitFramebuffer(0, 0, resx, resy,   0, 0, resx, resy,   GL_COLOR_BUFFER_BIT, GL_NEAREST);
        
        //
        glBindFramebuffer(GL_FRAMEBUFFER, 0);


        ImGui::Render();


        //
		glfwSwapBuffers(window);
	}

	//
	ImGui_ImplGlfwGL3_Shutdown();
    ImGui::DestroyContext();
	glfwTerminate();

	//
	return 0;
}

