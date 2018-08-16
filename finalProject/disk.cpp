#include <stdio.h>
#include "disk.h"
static const double PI = 3.1415926535897932384626433832795;

GLuint  diskVao;
GLuint  diskVbo;
GLuint  diskEbo;

int tick = 0;

GLuint diskIndices[NumIndices];
vec4 diskPoints[NumDiskPoints + 1];
vec3 diskNormals[NumDiskPoints + 1];
vec2 diskTex[NumDiskPoints + 1];

void createDisk();
void updateNormals();
void renderDisk();

void createDisk() {
	diskPoints[tick][0] = 0.0;
	diskPoints[tick][1] = -1.0;
	diskPoints[tick][2] = 0.0;
	diskPoints[tick][3] = 1.0;

	diskNormals[tick][0] = 0.0;
	diskNormals[tick][1] = 0.0;
	diskNormals[tick][2] = 0.0;

	diskTex[tick][0] = 0.5f;
	diskTex[tick][1] = 0.5f;

	tick++;

	int i;
	float theta;
	int tIndices = 0;


	for (i = 0; i < NumDiskPoints; ++i) {

		theta = (float)(i*20.0f*PI / 180.0f);

		diskPoints[tick][0] = cos(theta);
		diskPoints[tick][1] = -1.0;
		diskPoints[tick][2] = -sin(theta);
		diskPoints[tick][3] = 1.0;

		diskNormals[tick][0] = 0.0;
		diskNormals[tick][1] = 0.0;
		diskNormals[tick][2] = 0.0;

		diskTex[tick][0] = (1 + cos(theta)) / 2;
		diskTex[tick][1] = (1 + sin(theta)) / 2;

		if (i <= (NumDiskPoints - 2)) {

			diskIndices[tIndices] = 0; tIndices++;
			diskIndices[tIndices] = tick; tIndices++;
			diskIndices[tIndices] = tick + 1; tIndices++;

		}
		else {
			diskIndices[tIndices] = 0; tIndices++;
			diskIndices[tIndices] = tick; tIndices++;
			diskIndices[tIndices] = 1; tIndices++;
		}
		tick++;
	}

	updateVertexNormals();

	int offset = 0;

	glGenVertexArrays(1, &diskVao);
	glBindVertexArray(diskVao);
	glGenBuffers(1, &diskVbo);

	glBindBuffer(GL_ARRAY_BUFFER, diskVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(diskPoints) + sizeof(diskNormals) + sizeof(diskTex), NULL, GL_STATIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(diskPoints), (GLfloat *)diskPoints);
	offset += sizeof(diskPoints);
	glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(diskNormals), (GLfloat *)diskNormals);
	offset += sizeof(diskNormals);
	glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(diskTex), (GLfloat *)diskTex);

	glGenBuffers(1, &diskEbo);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, diskEbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(diskIndices), (diskIndices), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid *)sizeof(diskPoints));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid *)(sizeof(diskPoints) + sizeof(diskNormals)));
	glEnableVertexAttribArray(2);
	glBindVertexArray(0);
}

void updateVertexNormals() {
	vec4 P1, P2, P3;
	vec3 N;

	for (int i = 0;i < NumIndices;i += 3) {
		P1 = diskPoints[diskIndices[i]];
		P2 = diskPoints[diskIndices[i + 1]];
		P3 = diskPoints[diskIndices[i + 2]];

		N = normalize(cross(vec3(P2 - P1), vec3(P3 - P1)));

		diskNormals[diskIndices[i]] += N;
		diskNormals[diskIndices[i + 1]] += N;
		diskNormals[diskIndices[i + 2]] += N;
	}
	for (int i = 0;i < NumDiskPoints;i++) {
		diskNormals[i] = normalize(vec3(diskNormals[i]));
	}
}
void renderDisk() {
	glBindVertexArray(diskVao);
	glDrawElements(GL_TRIANGLES, NumIndices, GL_UNSIGNED_INT, NULL);
	glBindVertexArray(0);
}
