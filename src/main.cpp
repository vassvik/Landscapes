#include <stdio.h>

#include <glad/glad.c> // includes glad.h
#include <GLFW/glfw3.h>

#include "utils.hpp"
#include "imgui_impl_glfw_gl3.cpp"
 

int main() {
    // init and setup glfw
    if (!glfwInit()) {
        printf("Error, could not initialize GLFW\n");
        return -1;
    }

    //
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    int resx = 900, resy = 900;
    GLFWwindow *window = glfwCreateWindow(resx, resy, "Terrain Visualizer", NULL, NULL);

    if (window == NULL) {
        printf("Error, could not create window.\n");
        glfwTerminate();
        return -2;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync


    // init glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        printf("Could not load OpenGL functions.\n");
        return -3;
    }


    // init imgui
    ImGui::CreateContext();
    ImGui_ImplGlfwGL3_Init(window, true);
    //ImGui::StyleColorsDark();


    // load shaders
    GLuint program = load_shaders("src/vertex_shader.glsl", "src/fragment_shader.glsl");


    // define "terrain" mesh vertices
    Mesh mesh;
    int Nx = 20; // NOTE: Nx and Ny are number of quads in each direction
    int Ny = 20;

    int num_vertices = (Nx+1)*(Ny+1);
    Vec3 *base_vertices = new Vec3[3*num_vertices];

    float dx = 0.05;
    float dy = 0.05;
    float min_x = 0.0;
    float min_y = 0.0;
    float max_x = dx*Nx; 
    float max_y = dy*Ny;
    for (int j = 0; j <= Ny; j++) {
        for (int i = 0; i <= Nx; i++) {
            int k = j*(Nx+1) + i;
            float x = min_x + float(i)*dx - (max_x + min_x)/2.0;
            float y = min_y + float(j)*dy - (max_y + min_y)/2.0;
            base_vertices[k] = Vec3(x, y, 0.25*cos(2.0*3.14*x)*cos(2.0*3.14*y) + 0.125*cos(4.0*3.14*x+ 3.7*y)*cos(4.0*3.14*y - 6.0*x));
        }
    }

    // define mesh triangle vertices. each quad is 2 triangles each containing 3 vertices
    mesh.num_vertices = Nx*Ny*6;
    mesh.vertices = new Vertex[mesh.num_vertices];

    // for every quad
    for (int j = 0; j < Ny; j++) {
        for (int i = 0; i < Nx; i++) {
            int k = j*Nx + i;

            // grab the vertices
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

            // calculate face vector
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


    // define vao and vbo
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLuint vbo;
    glGenBuffers(1, &vbo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*mesh.num_vertices, &mesh.vertices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0); 
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, position)));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, normal)));


    // our framebuffer contains two textures
    //  - the "color" texture, which you draw colors to as per usual
    //  - the "pick" texture, which contains the id of the primitive covered by that pixel, 
    //    used for tracking which triangle the cursor is hvoering

    auto create_texture = [](GLenum internal_format, int width, int height) {
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        // we don't actually care about the content of the texture, as this is overwritten by the shader
        glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr); 

        return texture;
    };

    GLuint screen_texture = create_texture(GL_RGBA, resx, resy);
    GLuint pick_texture = create_texture(GL_RGBA32F, resx, resy);

    //  Setup framebuffer
    GLuint framebuffer;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screen_texture, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, pick_texture, 0);

    GLuint buffers[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, &buffers[0]);

    // we need a depth buffer as well
    GLuint depthrenderbuffer;
    glGenRenderbuffers(1, &depthrenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, resx, resy);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);

    // check if everything is alright
    int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) printf("ERR framebuffers\n"); 

    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    // helpers for mouse picking
    int hover_triangle = -1;
    Vec3 hover_position;

    float clicked_radius = 0.1;
    int clicked_triangle = -1;
    Vec3 clicked_position;

    //
    glEnable(GL_DEPTH_TEST);
    glClearColor(1.0, 0.7, 0.4 , 1.0);

    while (!glfwWindowShouldClose(window)) {
        // let GLFW handle OS events and update internal input data
        glfwPollEvents();

        // close the window at the end of the frame if Escape is pressed
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }

        // start new imgui frame
        ImGui_ImplGlfwGL3_NewFrame();
        ImGuiIO &io = ImGui::GetIO(); // input

        // camera
        static float dist = 4.0; 
        static float phi = 45.0; 
        static float theta = 90.0;   
        float fov = 45.0; 

        if (io.MouseDown[0]) {   
            phi -= io.MouseDelta.x;     
            theta -= io.MouseDelta.y;
            if (theta > 179.0) theta = 179.0;
            if (theta < 1.0) theta = 1.0;
        } 

        if (!io.KeyCtrl)
            dist *= pow(0.95, io.MouseWheel); 

        Vec3 pos = dist*Vec3(cos(PI/180.0*phi)*sin(PI/180.0*theta), sin(PI/180.0*phi)*sin(PI/180.0*theta), cos(PI/180.0*theta));
        Vec3 at = Vec3(0.0, 0.0, 0.0);
        Vec3 f = normalize(Vec3(at.x - pos.x, at.y - pos.y, at.z - pos.z));
        Vec3 u = normalize(Vec3(0.0, 0.0, 1.0));
        Vec3 r = normalize(cross(f,u));
        u = cross(r,f);


        // define matrices
        Mat4 View = look_at(pos, at, Vec3(0.0, 0.0, 1.0));    
        Mat4 Projection = perspective(fov, resx/float(resy), 0.1, 1000.0);
        Mat4 Model;
        Mat4 MVP = Projection*View*Model;    


        // set uniforms
        glUseProgram(program);

        glUniformMatrix4fv(glGetUniformLocation(program, "M"), 1, GL_FALSE, &Model.M[0][0]); 
        glUniformMatrix4fv(glGetUniformLocation(program, "MVP"), 1, GL_FALSE, &MVP.M[0][0]); 

        glUniform1i(glGetUniformLocation(program, "hover_triangle"), hover_triangle);
        glUniform3f(glGetUniformLocation(program, "hover_position"), hover_position.x, hover_position.y, hover_position.z);

        glUniform1i(glGetUniformLocation(program, "clicked_triangle"), clicked_triangle);
        glUniform3f(glGetUniformLocation(program, "clicked_position"), clicked_position.x, clicked_position.y, clicked_position.z);
        glUniform1f(glGetUniformLocation(program, "clicked_radius"), clicked_radius);

        // render to framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, mesh.num_vertices);


        // picking, read from the second texture under the mouse cursor
        // note that this will be 1 frame behind due to having to draw to know what part is below the cursor
        glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer);
        glReadBuffer(GL_COLOR_ATTACHMENT1);

        // get mouse cursor position
        double mx, my;
        glfwGetCursorPos(window, &mx, &my);

        // extract primitive id below cursor
        float data[4];
        glReadPixels(mx, resy-my-1, 1, 1, GL_RGBA, GL_FLOAT, &data[0]);
        
        // check for collision
        bool hit_terrain = !((fabs(data[0] - 1.0) < 1.0e-4 && fabs(data[1] - 0.7) < 1.0e-4 && fabs(data[2] - 0.4) < 1.0e-4));

        //  picking nad sphere of influence input
        if (hit_terrain) {
            hover_triangle = int(data[0] + 0.5);
            hover_position = Vec3{data[1], data[2], data[3]};

            if (ImGui::IsMouseClicked(0)) {
                clicked_triangle = int(data[0] + 0.5);
                clicked_position = Vec3{data[1], data[2], data[3]};
            } 
        } else {
            hover_triangle = -1;
            hover_position = Vec3{0.0, 0.0, 0.0};
        }


        if (ImGui::IsMouseReleased(0)) {
            clicked_triangle = int(-1);
            clicked_position = Vec3{0.0, 0.0, 0.0};
        }

        if (io.KeyCtrl) {
            clicked_radius += ImGui::GetIO().MouseWheel*0.01;
            if (clicked_radius < 0.0) clicked_radius = 0.0;
            if (clicked_radius > 0.5) clicked_radius = 0.5;
        }

        // output to imgui 
        ImGui::Text("data = %f %f %f %f %d", data[0], data[1], data[2], data[3], hit_terrain);

        if (hover_triangle != -1) {
            ImGui::Text("Hover: %d at {%f %f %f}", hover_triangle, hover_position.x, hover_position.y, hover_position.z);
        } else {
            ImGui::Text("Hover: None");
        }
        
        if (clicked_triangle != -1) {
            ImGui::Text("Clicked: %d at {%f %f %f}", clicked_triangle, clicked_position.x, clicked_position.y, clicked_position.z);
        } else {
            ImGui::Text("Clicked: None");
        }


        // blit the color texture onto the screen
        glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer);
        glReadBuffer(GL_COLOR_ATTACHMENT0);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glBlitFramebuffer(0, 0, resx, resy,   0, 0, resx, resy,   GL_COLOR_BUFFER_BIT, GL_NEAREST);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // finally render imgui on top
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
