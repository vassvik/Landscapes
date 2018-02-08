#include <stdio.h>
#include <stdlib.h>
#include <glad/glad.h>

#include "utils.hpp"


// Shader utility functions
char *read_entire_file(const char *filename) {
    FILE *f = fopen(filename, "rb");

    if (f == NULL) {
        return NULL;
    }

    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *string = (char*)malloc(fsize + 1);
    fread(string, fsize, 1, f);
    string[fsize] = '\0';
    fclose(f);

    return string;
}

int compile_shader(const char *file_path, GLuint shader_ID) {
    char *shader_code = read_entire_file(file_path);
    if (shader_code == NULL) {
        fprintf(stderr, "Error: Could not read shader file: \"%s\"\n", file_path);
        return -1;
    }
    // Compile Shader
    printf("Compiling shader : %s\n", file_path);
    glShaderSource(shader_ID, 1, (const char**)&shader_code , NULL);
    glCompileShader(shader_ID);

    // Check Shader
    GLint result;
    glGetShaderiv(shader_ID, GL_COMPILE_STATUS, &result);

    if ( result == GL_FALSE ){
        GLint info_log_length;
        glGetShaderiv(shader_ID, GL_INFO_LOG_LENGTH, &info_log_length);

        char shader_error_message[9999];
        glGetShaderInfoLog(shader_ID, info_log_length, NULL, shader_error_message);
        fprintf(stderr, "Error while compiling shader \"%s\":\n%s", file_path, shader_error_message);

        free(shader_code);
        return -2;
    }

    free(shader_code);

    return 0;
}

GLuint load_shaders(const char *vertex_file_path, const char *fragment_file_path) {
    // create shaders
    GLuint vertex_shader_ID   = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragment_shader_ID = glCreateShader(GL_FRAGMENT_SHADER);

    // compile and check shaders
    int err1 = compile_shader(vertex_file_path, vertex_shader_ID);
    int err2 = compile_shader(fragment_file_path, fragment_shader_ID);

    if (err1 || err2) {
        glDeleteShader(vertex_shader_ID);
        glDeleteShader(fragment_shader_ID);
        return 0;
    }

    // create program, attach and link the shaders
    GLuint program_ID = glCreateProgram();
    glAttachShader(program_ID, vertex_shader_ID);
    glAttachShader(program_ID, fragment_shader_ID);
    glLinkProgram(program_ID);

    // Check program
    GLint result;
    glGetProgramiv(program_ID, GL_LINK_STATUS, &result);

    if ( result == GL_FALSE ){
        GLint info_log_length;
        glGetProgramiv(program_ID, GL_INFO_LOG_LENGTH, &info_log_length);

        GLchar program_error_message[9999];
        glGetProgramInfoLog(program_ID, info_log_length, NULL, program_error_message);
        printf("Error while linking program:\n%s\n", program_error_message);
        
        glDeleteShader(vertex_shader_ID);
        glDeleteShader(fragment_shader_ID);
        return 0;
    }

    // Cleanup shaders
    glDeleteShader(vertex_shader_ID);
    glDeleteShader(fragment_shader_ID);

    return program_ID;
}










////////////////////////////////////////////////////////////////////////////////
/// 3D Vector
////////////////////////////////////////////////////////////////////////////////

Vec3::Vec3() : x(0.0), y(0.0), z(0.0) {

}

Vec3::Vec3(float x, float y, float z) : x(x), y(y), z(z) {

}

float& Vec3::operator[](int rhs) {
    return xyz[rhs];
}

const float& Vec3::operator[](int rhs) const {
    return xyz[rhs];
}

Vec3  operator+(Vec3 lhs, Vec3 rhs) { return Vec3{lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z}; }
Vec3  operator-(Vec3 lhs, Vec3 rhs) { return Vec3{lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z}; }
Vec3  operator*(Vec3 lhs, Vec3 rhs) { return Vec3{lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z}; }
Vec3  operator/(Vec3 lhs, Vec3 rhs) { return Vec3{lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z}; }

Vec3  operator+(float lhs, Vec3 rhs) { return Vec3{lhs   + rhs.x, lhs   + rhs.y, lhs   + rhs.z}; }
Vec3  operator+(Vec3 lhs, float rhs) { return Vec3{lhs.x + rhs,   lhs.y + rhs,   lhs.z + rhs}; }

Vec3  operator-(float lhs, Vec3 rhs) { return Vec3{lhs   - rhs.x, lhs   - rhs.y, lhs   - rhs.z}; }
Vec3  operator-(Vec3 lhs, float rhs) { return Vec3{lhs.x - rhs,   lhs.y - rhs,   lhs.z - rhs}; }

Vec3  operator*(float lhs, Vec3 rhs) { return Vec3{lhs   * rhs.x, lhs   * rhs.y, lhs   * rhs.z}; }
Vec3  operator*(Vec3 lhs, float rhs) { return Vec3{lhs.x * rhs,   lhs.y * rhs,   lhs.z * rhs}; }

Vec3  operator/(float lhs, Vec3 rhs) { return Vec3{lhs / rhs.x, lhs / rhs.y, lhs / rhs.z}; }
Vec3  operator/(Vec3 lhs, float rhs) { return Vec3{lhs.x / rhs,   lhs.y / rhs,   lhs.z / rhs}; }

bool operator!=(Vec3 lhs, Vec3 rhs) { return !(lhs == rhs); }
bool operator==(Vec3 lhs, Vec3 rhs) { return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z; }

float dot(Vec3 u, Vec3 v) {
    return u.x*v.x + u.y*v.y + u.z*v.z;
}

float length(Vec3 u) {
    return sqrt(u.x*u.x + u.y*u.y + u.z*u.z);
}

float distance(Vec3 u, Vec3 v) {
    return sqrt((u.x - v.x)*(u.x - v.x) + (u.y - v.y)*(u.y - v.y) + (u.z - v.z)*(u.z - v.z));
}

Vec3 abs(Vec3 u) {
    return Vec3{(float)fabs(u.x), (float)fabs(u.y), (float)fabs(u.z)};
}

Vec3 cross(Vec3 u, Vec3 v) {
    return Vec3{u.y*v.z - u.z*v.y, u.z*v.x - u.x*v.z, u.x*v.y - u.y*v.x};
}

Vec3 normalize(Vec3 u) {
    float l = length(u);
    return u/l;
}

Vec3 rotate_axis(Vec3 u, Vec3 axis, float angle) {
    Vec3 a = normalize(axis);

    float c = cos(angle);
    float s = sin(angle);
    float oc = 1.0 - c;

    return Vec3{(oc * a.x * a.x + 1.f * c)*u.x + (oc * a.x * a.y - a.z * s)*u.y + (oc * a.z * a.x + a.y * s)*u.z,
                 (oc * a.x * a.y + a.z * s)*u.x + (oc * a.y * a.y + 1.f * c)*u.y + (oc * a.y * a.z - a.x * s)*u.z,
                 (oc * a.z * a.x - a.y * s)*u.x + (oc * a.y * a.z + a.x * s)*u.y + (oc * a.z * a.z + 1.f * c)*u.z};
}

Vec3 rotateX(Vec3 v, float angle) {
    float c = cos(angle);
    float s = sin(angle);

    return Vec3{v.x, v.y*c - v.z*s, v.y*s + v.z*c};
}

Vec3 rotateY(Vec3 v, float angle) {
    float c = cos(angle);
    float s = sin(angle);

    return Vec3{v.x*c + v.z*s, v.y, -v.x*s + v.z*c};
}

Vec3 rotateZ(Vec3 v, float angle) {
    float c = cos(angle);
    float s = sin(angle);

    return Vec3{v.x*c - v.y*s, v.x*s + v.y*c, v.z};
}


Vec3 swizzle(Vec3 u, int a, int b, int c) {
    return Vec3{u[a], u[b], u[c]};
}


////////////////////////////////////////////////////////////////////////////////
/// 4x4 Matrix
////////////////////////////////////////////////////////////////////////////////

Mat4 :: Mat4() {
    M[0][0] = M[1][1] = M[2][2] = M[3][3] = 1.0;

}

Mat4 operator*(Mat4 lhs, Mat4 rhs) {
    Mat4 out;
    for (int j = 0; j < 4; j++) {
        for (int i = 0; i < 4; i++) {
            out.M[j][i] = 0.0;
            for (int k = 0; k < 4; k++) {
                out.M[j][i] += lhs.M[k][i]*rhs.M[j][k];
            }
        }
    }
    return out;
}

Mat4 translate(Vec3 move) {
    Mat4 out;
    out.M[3][0] = move.x, out.M[3][1] = move.y, out.M[3][2] = move.z;
    return out;
}

Mat4 scale(Vec3 scale) {
    Mat4 out;
    out.M[0][0] = scale.x;
    out.M[1][1] = scale.y;
    out.M[2][2] = scale.z;
    return out;
}

Mat4 rotate(Vec3 axis, float angle) {
    axis = normalize(axis);
    float c = cos(-angle*PI/180.0); // negative because of column-major
    float s = sin(-angle*PI/180.0); // negative because of column-major
    float u = axis.x;
    float v = axis.y;
    float w = axis.z;

    Mat4 mat;
    mat.M[0][0] =      c + u*u*(1.0 - c);
    mat.M[1][0] =  w * s + v*u*(1.0 - c);
    mat.M[2][0] = -v * s + w*u*(1.0 - c);
    mat.M[0][1] = -w * s + u*v*(1.0 - c);
    mat.M[1][1] =      c + v*v*(1.0 - c);
    mat.M[2][1] =  u * s + w*v*(1.0 - c);
    mat.M[0][2] =  v * s + u*w*(1.0 - c);
    mat.M[1][2] = -u * s + v*w*(1.0 - c);
    mat.M[2][2] =      c + w*w*(1.0 - c);

    return mat;
}

Mat4 view(Vec3 r, Vec3 u, Vec3 f, Vec3 p) {
    Mat4 mat;
    mat.M[0][0] =  r.x, mat.M[1][0] =  r.y, mat.M[2][0] =  r.z, mat.M[3][0] = -dot(r, p);
    mat.M[0][1] =  u.x, mat.M[1][1] =  u.y, mat.M[2][1] =  u.z, mat.M[3][1] = -dot(u, p);
    mat.M[0][2] = -f.x, mat.M[1][2] = -f.y, mat.M[2][2] = -f.z, mat.M[3][2] =  dot(f, p);

    return mat;
}

Mat4 look_at(Vec3 eye, Vec3 center, Vec3 up) {
    Vec3 f = normalize(Vec3(center.x - eye.x, center.y - eye.y, center.z - eye.z));
    Vec3 u = normalize(up);
    Vec3 r = normalize(cross(f,u));
    u = cross(r,f);

    return view(r, u ,f, eye);
}

Mat4 perspective(float fov, float ratio, float zmin, float zmax) {
    Mat4 out;
    float f = 1.0/tan((fov*PI/180.0)/2.0);
    out.M[0][0] = f/ratio; 
    out.M[1][1] = f;
    out.M[2][2] = (zmax + zmin)/(zmin - zmax);
    out.M[3][2] = 2*zmax*zmin/(zmin - zmax);
    out.M[2][3] = -1.0;
    out.M[3][3] = 0.0;

    return out;
}
