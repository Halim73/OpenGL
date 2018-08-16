#include <GL/glew.h>
#include <GL/freeglut.h>
#define GLM_FORCE_RADIANS

#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#define NumConePoints  18
#define NumTriangles   18
#define NumIndices     3*NumTriangles

extern GLuint  cone_vao;
extern GLuint  coneVbo;
extern GLuint  coneEbo;

void createCone();
void updateNormals();
void renderCone();