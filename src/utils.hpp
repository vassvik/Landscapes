#include <vector>

struct Vec2 {
    float x, y;
};

struct Vec3 {
    float x, y, z;
};

Vec3 operator-(Vec3 lhs, Vec3 rhs);
Vec3 cross(Vec3 u, Vec3 v);
Vec3 normalize(Vec3 u);
float dot(Vec3 u, Vec3 v);

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
