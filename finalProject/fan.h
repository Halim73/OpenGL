#pragma once
#include <GL/glew.h>
#include <GL/freeglut.h>
#define GLM_FORCE_RADIANS

#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

extern GLuint fanVao;
extern GLuint fanVbo;
extern GLuint fanEbo;

void createFanBlade();
void renderFan();