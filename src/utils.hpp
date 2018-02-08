#include <vector>


#ifndef PI
#define PI 3.14159265358979323846
#endif

////////////////////////////////////////////////////////////////////////////////
/// 3D Vector
////////////////////////////////////////////////////////////////////////////////

struct Vec3 {
    union {
        struct { float x, y, z; };
        float xyz[3];
    };
    Vec3();
    Vec3(float x, float y, float z);
    float& operator[](int rhs);
    const float& operator[](int rhs) const;
};

Vec3 operator+(Vec3 lhs, Vec3 rhs);
Vec3 operator-(Vec3 lhs, Vec3 rhs);
Vec3 operator*(Vec3 lhs, Vec3 rhs);
Vec3 operator/(Vec3 lhs, Vec3 rhs);

Vec3 operator+(float lhs, Vec3 rhs);
Vec3 operator+(Vec3 lhs, float rhs);

Vec3 operator-(float lhs, Vec3 rhs);
Vec3 operator-(Vec3 lhs, float rhs);

Vec3 operator*(float lhs, Vec3 rhs);
Vec3 operator*(Vec3 lhs, float rhs);

Vec3 operator/(float lhs, Vec3 rhs);
Vec3 operator/(Vec3 lhs, float rhs);

bool operator!=(Vec3 lhs, Vec3 rhs);
bool operator==(Vec3 lhs, Vec3 rhs);

float dot(Vec3 u, Vec3 v);
float length(Vec3 u);
float distance(Vec3 u, Vec3 v);

Vec3 abs(Vec3 u);
Vec3 cross(Vec3 u, Vec3 v);
Vec3 normalize(Vec3 v);
Vec3 swizzle(Vec3 u, int a, int b, int c);
Vec3 rotate_axis(Vec3 v, Vec3 axis, float angle);
Vec3 rotateX(Vec3 v, float angle);
Vec3 rotateY(Vec3 v, float angle);
Vec3 rotateZ(Vec3 v, float angle);


////////////////////////////////////////////////////////////////////////////////
/// 4x4 Matrix
////////////////////////////////////////////////////////////////////////////////

struct Mat4 {
    float M[4][4];// = {0};

    Mat4();  
};

Mat4 operator*(Mat4 lhs, Mat4 rhs); // matrix-matrix product

Mat4 translate(Vec3 move);
Mat4 scale(Vec3 scale);
Mat4 rotate(Vec3 axis, float angle);

Mat4 view(Vec3 r, Vec3 u, Vec3 f, Vec3 p);
Mat4 look_at(Vec3 eye, Vec3 center, Vec3 up);
Mat4 perspective(float fov, float ratio, float zmin, float zmax);



struct Vertex {
	Vec3 position;
	Vec3 normal;
};

struct Mesh {
	int num_vertices;
	Vertex *vertices;
};

char *read_entire_file(const char *filename);
int compile_shader(const char *file_path, GLuint shader_ID);
GLuint load_shaders(const char *vertex_file_path, const char *fragment_file_path);





