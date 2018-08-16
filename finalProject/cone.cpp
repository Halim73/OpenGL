#include <stdio.h>
#include "cone.h"
static const double PI = 3.1415926535897932384626433832795;

GLuint  coneVao;
GLuint  coneVbo;
GLuint  coneEbo;

int count = 0;

GLuint coneIndices[NumIndices];
vec4 conePoints[NumConePoints + 1];
vec3 coneNormals[NumConePoints + 1];
vec2 coneTex[NumConePoints + 1];

void createCone();
void updateNormals();
void renderCone();

void createCone() {
	conePoints[count][0] = 0.0;
	conePoints[count][1] = 1.0;
	conePoints[count][2] = 0.0;
	conePoints[count][3] = 1.0;

	coneNormals[count][0] = 0.0;
	coneNormals[count][1] = 0.0;
	coneNormals[count][2] = 0.0;

	coneTex[count][0] = 0.5f;
	coneTex[count][1] = 0.5f;

	count++;

	int i;
	float theta;
	int tIndices = 0;


	for (i = 0; i < NumConePoints; ++i) {

		theta = (float)(i*20.0f*PI / 180.0f);

		conePoints[count][0] = cos(theta);
		conePoints[count][1] = -2.0;
		conePoints[count][2] = -sin(theta);
		conePoints[count][3] = 2.0;

		coneNormals[count][0] = 0.0;
		coneNormals[count][1] = 0.0;
		coneNormals[count][2] = 0.0;

		coneTex[count][0] = (1 + cos(theta)) / 2;
		coneTex[count][1] = (1 + sin(theta)) / 2;

		if (i <= (NumConePoints - 2)) {

			coneIndices[tIndices] = 0; tIndices++;
			coneIndices[tIndices] = count; tIndices++;
			coneIndices[tIndices] = count + 1; tIndices++;

		}
		else {
			coneIndices[tIndices] = 0; tIndices++;
			coneIndices[tIndices] = count; tIndices++;
			coneIndices[tIndices] = 1; tIndices++;
		}
		count++;
	}

	updateNormals();

	int offset = 0;

	glGenVertexArrays(1, &coneVao);
	glBindVertexArray(coneVao);
	glGenBuffers(1, &coneVbo);

	glBindBuffer(GL_ARRAY_BUFFER, coneVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(conePoints) + sizeof(coneNormals) + sizeof(coneTex), NULL, GL_STATIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(conePoints), (GLfloat *)conePoints);
	offset += sizeof(conePoints);
	glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(coneNormals), (GLfloat *)coneNormals);
	offset += sizeof(coneNormals);
	glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(coneTex), (GLfloat *)coneTex);

	glGenBuffers(1, &coneEbo);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, coneEbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(coneIndices), (coneIndices), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid *)sizeof(conePoints));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid *)(sizeof(conePoints) + sizeof(coneNormals)));
	glEnableVertexAttribArray(2);
	glBindVertexArray(0);
}

void updateNormals() {
	vec4 P1, P2, P3;
	vec3 N;

	for (int i = 0;i < NumIndices;i += 3) {
		P1 = conePoints[coneIndices[i]];
		P2 = conePoints[coneIndices[i + 1]];
		P3 = conePoints[coneIndices[i + 2]];

		N = normalize(cross(vec3(P2 - P1), vec3(P3 - P1)));

		coneNormals[coneIndices[i]] += N;
		coneNormals[coneIndices[i + 1]] += N;
		coneNormals[coneIndices[i + 2]] += N;
	}
	for (int i = 0;i < NumConePoints;i++) {
		coneNormals[i] = normalize(vec3(coneNormals[i]));
	}
}
void renderCone() {
	glBindVertexArray(coneVao);
	glDrawElements(GL_TRIANGLES, NumIndices, GL_UNSIGNED_INT, NULL);
	glBindVertexArray(0);
}
