#include "fan.h"
GLuint fanVao;
GLuint fanVbo;
GLuint fanEbo;

void createFanBlade() {
	GLfloat vertices[]{
		-0.5,0.5,0.0,1.0,
		0.5, 0.5,0.0,1.0,
		0.0, 0.5,0.0,1.0
	};
	GLfloat normals[]{
		0.0,0.0,1.0,
		0.0,0.0,1.0,
		0.0,0.0,1.0
	};
	GLfloat indices[]{
		1,2,3
	};

	glGenVertexArrays(1, &fanVao);
	glBindVertexArray(fanVao);

	unsigned int call[3];
	glGenBuffers(3, call);

	glBindBuffer(GL_ARRAY_BUFFER, call[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)0, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);  // Vertex position

	glBindBuffer(GL_ARRAY_BUFFER, call[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);  // 

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, call[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glBindVertexArray(0);
}
void renderFan() {
	glBindVertexArray(fanVao);
	glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
}