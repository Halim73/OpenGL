#include <GL/glew.h>
#include <GL/freeglut.h>

#include <stdlib.h>
#include <iostream>
#include <math.h>
#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>

#include "grass.h"
#include "ground.h"
#include "houseBody.h"
#include "cone.h"
#include "pyramid.h"
#include "disk.h"
#include "fan.h"

#define GLM_FORCE_RADIANS 

#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp> 

using namespace glm;

void Initialize(void);
void Display(void);
void makeTrees(void);
void makeHouse(void);
void makeDisk(void);
void makeWindmill(void);
void makePyramid(void);
void makeLake(void);
void setMatrices(void);
void Rotate(int);
void keyboard(unsigned char,int,int);

unsigned int loadTexture(char *);

GLuint prog;
GLuint render_prog;
GLuint program;

typedef struct {
	GLenum       type;
	const char*  filename;
	GLuint       shader;
} ShaderInfo;


float angle = 0.0;
float rotateAngle = 0.0;
float viewAngle = 0.0;

vec4 light_position(20.0, 40.0, 60.0, 0.0);  // directional light source

float material_shininess = 50.0;

vec4 ambient_product;
vec4 diffuse_product;
vec4 specular_product;

mat4 view_matrix;
mat4 projection_matrix;
mat4 model_matrix;
GLuint model_matrix_loc;
GLuint view_matrix_loc;
GLuint projection_matrix_loc;

mat4 view;
mat4 model;
mat4 projection;
mat4 model_view;
mat4 transformation_matrix;
mat4 mvp;
mat3 normalmatrix;
float aspect;
float speed = 100;

mat4 matRotate;
mat4 matTranslate;
mat4 matScaled;

GLuint boxID;
GLuint pyramidID;
GLuint treeID;
GLuint leafID;
GLuint roofID;
GLuint fanID;
GLuint windmillID;
GLuint chichenItza;
//----------------------------------------------------------------------------

const GLchar* ReadShader(const char* filename) {
#ifdef WIN32
	FILE* infile;
	fopen_s(&infile, filename, "rb");

#else
	FILE* infile = fopen(filename, "rb");
#endif // WIN32

	if (!infile) {
#ifdef _DEBUG
		printf("Unable to open file %s", filename);
#endif /* DEBUG */
		return NULL;
	}

	fseek(infile, 0, SEEK_END);
	int len = ftell(infile);
	fseek(infile, 0, SEEK_SET);

	GLchar* source = (GLchar*)malloc(sizeof(GLchar)*(len + 1));

	fread(source, 1, len, infile);
	fclose(infile);

	source[len] = 0;

	return ((GLchar*)source);
}

//----------------------------------------------------------------------------

GLuint LoadShaders(ShaderInfo* shaders){
	if (shaders == NULL) { return 0; }

	GLuint program = glCreateProgram();

	ShaderInfo* entry = shaders;
	while (entry->type != GL_NONE) {
		GLuint shader = glCreateShader(entry->type);

		entry->shader = shader;

		const GLchar* source = ReadShader(entry->filename);

		if (source == NULL) {


			for (entry = shaders; entry->type != GL_NONE; ++entry) {
				glDeleteShader(entry->shader);
				entry->shader = 0;
			}

			return 0;
		}

		glShaderSource(shader, 1, &source, NULL);
		free((GLchar*)source);

		glCompileShader(shader);

		GLint compiled;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
		if (!compiled) {
#ifdef _DEBUG
			GLsizei len;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);

			GLchar* log = (GLchar*)malloc(sizeof(GLchar)*(len + 1));
			glGetShaderInfoLog(shader, len, &len, log);
			printf("Shader compilation failed.\n");
			free(log);
#endif /* DEBUG */

			return 0;
		}

		glAttachShader(program, shader);

		++entry;
	}

	glLinkProgram(program);

	GLint linked;
	glGetProgramiv(program, GL_LINK_STATUS, &linked);
	if (!linked) {
#ifdef _DEBUG
		GLsizei len;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);

		GLchar* log = (GLchar*)malloc(sizeof(GLchar)*(len + 1));
		glGetProgramInfoLog(program, len, &len, log);
		printf("Shader linking failed: %s\n", log);
		free(log);
#endif /* DEBUG */

		for (entry = shaders; entry->type != GL_NONE; ++entry) {
			glDeleteShader(entry->shader);
			entry->shader = 0;
		}

		return 0;
	}

	return program;
}
////////////////////////////////////////////////////////////////////////////////////////
/********************************************************************************/

void Reshape(int width, int height) {

	glViewport(0, 0, width, height);
	aspect = float(width) / float(height);
}
/*****************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////


void Initialize(void)
{
	// Create the program for rendering the model
	
	// Initialize shader lighting parameters

	
	ShaderInfo shader[] = { { GL_VERTEX_SHADER, "grassOne.VERT" },
	{ GL_FRAGMENT_SHADER, "grassOne.FRAG" },
	{ GL_NONE, NULL }
	};
	
	render_prog = LoadShaders(shader);
	glUseProgram(render_prog);
	
	createGrass();

	ShaderInfo common_shaders[] = { { GL_VERTEX_SHADER, "common.vs" },
	                                { GL_FRAGMENT_SHADER, "common.fs" },
	                                { GL_NONE, NULL } };

	prog = LoadShaders(common_shaders);

	
	glUseProgram(prog);


	createGround();
	
	createBox();
	createCone();
	createPyramid();
	createDisk();
	createFanBlade();
	ShaderInfo textureShader[] = { { GL_VERTEX_SHADER, "texture.vs" },
	{ GL_FRAGMENT_SHADER, "texture.fs" },
	{ GL_NONE, NULL }
	};

	program = LoadShaders(textureShader);

	glUseProgram(program);
	
	//glActiveTexture(GL_TEXTURE0);

	glClearColor(0.8f, 0.5f, 0.32f, 1.0f);  // sky color
}

/**************************************************************************************************************/
unsigned int loadTexture(char* filename) {

	ILboolean success;
	unsigned int imageID;


	// Load Texture Map
	ilGenImages(1, &imageID);

	ilBindImage(imageID); /* Binding of DevIL image name */
	ilEnable(IL_ORIGIN_SET);
	ilOriginFunc(IL_ORIGIN_UPPER_LEFT);
	success = ilLoadImage((ILstring)filename);

	if (!success) {
		printf("Couldn't load the following texture file: %s", filename);
		// The operation was not sucessfull hence free image and texture 
		ilDeleteImages(1, &imageID);
		return 0;
	}

	// add information to the log
	ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);

	GLuint tid;
	glGenTextures(1, &tid);
	glBindTexture(GL_TEXTURE_2D, tid);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), 0,
		GL_RGBA, GL_UNSIGNED_BYTE, ilGetData());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


	float color[] = { 1.0f, 0.0f, 0.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color);


	glBindTexture(GL_TEXTURE_2D, 0);

	/* Because we have already copied image data into texture data
	we can release memory used by image. */
	ilDeleteImages(1, &imageID);
	return tid;
}

void Display(void){

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	glUseProgram(render_prog);

	vec4 light_ambient(0.5, 0.5, 1.0, 1.0);
	vec4 light_diffuse(0.5, 0.5, 1.0, 1.0);
	vec4 light_specular(1.0, 1.0, 1.0, 1.0);

	vec4 material_ambient(0.1, 1.0, 0.1, 1.0);
	vec4 material_diffuse(0.1, 1.0, 0.1, 1.0);
	vec4 material_specular(1.0, 1.0, 1.0, 1.0);

	ambient_product = light_ambient*material_ambient;
	diffuse_product = light_diffuse*material_diffuse;
	specular_product = light_specular*material_specular;

	glUniform4fv(glGetUniformLocation(render_prog, "AmbientProduct"), 1, (GLfloat*)&ambient_product[0]);
	glUniform4fv(glGetUniformLocation(render_prog, "DiffuseProduct"), 1, (GLfloat*)&diffuse_product[0]);
	glUniform4fv(glGetUniformLocation(render_prog, "SpecularProduct"), 1, (GLfloat*)&specular_product[0]);

	glUniform4fv(glGetUniformLocation(render_prog, "LightPosition"), 1, (GLfloat*)&light_position[0]);
	glUniform1f(glGetUniformLocation(render_prog, "Shininess"), material_shininess);

	view_matrix_loc = glGetUniformLocation(render_prog, "view_matrix");
	projection_matrix_loc = glGetUniformLocation(render_prog, "projection_matrix");
	model_matrix_loc = glGetUniformLocation(render_prog, "model_matrix");
	view_matrix = glm::lookAt(glm::vec3(0.0f, 40.0f, 200.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	glUniformMatrix4fv(view_matrix_loc, 1, GL_FALSE, (GLfloat*)&view_matrix[0]);
	projection_matrix = perspective(radians(60.0f), aspect, 1.0f, 3000.0f);
	model_matrix = rotate(mat4(1.0f), radians(0.0f), vec3(0.0f, 1.0f, 0.0f));

	glUniformMatrix4fv(model_matrix_loc, 1, GL_FALSE, (GLfloat*)&model_matrix[0]);
	glUniformMatrix4fv(projection_matrix_loc, 1, GL_FALSE, (GLfloat*)&projection_matrix[0]);

	renderGrass();
	
	glUseProgram(prog);

	material_ambient = vec4(0.25, 0.25, 0.25, 1.0);
	material_diffuse = vec4(0.25, 0.25, 0.25, 1.0);
	material_specular = vec4(0.25, 0.25, 0.25, 1.0);

	ambient_product = light_ambient*material_ambient;
	diffuse_product = light_diffuse*material_diffuse;
	specular_product = light_specular*material_specular;
	glUniform4fv(glGetUniformLocation(prog, "AmbientProduct"), 1, (GLfloat*)&ambient_product[0]);
	glUniform4fv(glGetUniformLocation(prog, "DiffuseProduct"), 1, (GLfloat*)&diffuse_product[0]);
	glUniform4fv(glGetUniformLocation(prog, "SpecularProduct"), 1, (GLfloat*)&specular_product[0]);

	glUniform4fv(glGetUniformLocation(prog, "LightPosition"), 1, (GLfloat*)&light_position[0]);
	glUniform1f(glGetUniformLocation(prog, "Shininess"), material_shininess);

	view_matrix_loc = glGetUniformLocation(prog, "view_matrix");
	projection_matrix_loc = glGetUniformLocation(prog, "projection_matrix");
	model_matrix_loc = glGetUniformLocation(prog, "model_matrix");
	view_matrix = glm::lookAt(glm::vec3(0.0f, 40.0f, 200.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	projection_matrix = perspective(radians(60.0f), aspect, 1.0f, 3000.0f);

	glUniformMatrix4fv(view_matrix_loc, 1, GL_FALSE, (GLfloat*)&view_matrix[0]);
	glUniformMatrix4fv(model_matrix_loc, 1, GL_FALSE, (GLfloat*)&model_matrix[0]);
	glUniformMatrix4fv(projection_matrix_loc, 1, GL_FALSE, (GLfloat*)&projection_matrix[0]);
	
	
	renderGround();
	view = glm::lookAt(vec3(0.0f, 60.0f, 200.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));

	glUseProgram(program);
	
	vec3 light_intensity(0.5f, 0.4f, 0.3f);
	vec4 light_position(40.0f, 40.0f, 6000.0f, 0.0f);
	//vec4 light_position(0.0f, 500.0f, 500.0f, 0.0f);
	vec3 materialAmbient(0.3, 0.3, 0.3);
	vec3 materialDiffuse(0.9, 0.9, 0.9);
	vec3 materialSpecular(0.9, 0.9, 0.9);

	GLfloat shininess = 100.0f;

	glUniform3fv(glGetUniformLocation(program, "Light.Intensity"), 1, (GLfloat*)&light_intensity);
	glUniform4fv(glGetUniformLocation(program, "Light.Position"), 1, (GLfloat*)&light_position);
	glUniform3fv(glGetUniformLocation(program, "Material.Ka"), 1, (GLfloat*)&materialAmbient);
	glUniform3fv(glGetUniformLocation(program, "Material.Kd"), 1, (GLfloat*)&materialDiffuse[0]);
	glUniform3fv(glGetUniformLocation(program, "Material.Ks"), 1, (GLfloat*)&materialSpecular[0]);
	glUniform1f(glGetUniformLocation(program, "Material.Shininess"), shininess);
	glUniform1i(glGetUniformLocation(program, "Tex1"),0);
	boxID = loadTexture((char *)"houseTexture.jpg");
	pyramidID = loadTexture((char *)"roof.jpg");
	leafID = loadTexture((char *)"greenPine.jpg");
	treeID = loadTexture((char *)"wood.jpg");
	roofID = loadTexture((char *)"frontend.jpg");
	fanID = loadTexture((char *)"gold.jpg");
	windmillID = loadTexture((char *)"woodHouse.jpg");
	chichenItza = loadTexture((char *)"darkRock.jpg");

	makeHouse();

	makeTrees();

	makeDisk();
	
	makeWindmill();

	makePyramid();

	//makeLake();

	glutSwapBuffers();

}
void setMatrices() {
	mvp = mat4(1.0f);
	model_view = view * model_matrix;
	normalmatrix = mat3(model_view);
	projection = glm::perspective(radians(60.0f), aspect, 1.5f, 1000.0f);
	mvp = projection * model_view;

	glUniformMatrix4fv(glGetUniformLocation(program, "ModelViewMatrix"), 1, GL_FALSE, (GLfloat*)&model_view[0]);
	glUniformMatrix3fv(glGetUniformLocation(program, "NormalMatrix"), 1, GL_FALSE, (GLfloat*)&normalmatrix[0]);
	glUniformMatrix4fv(glGetUniformLocation(program, "MVP"), 1, GL_FALSE, (GLfloat*)&mvp[0]);
	glUniformMatrix4fv(glGetUniformLocation(program, "ProjectionMatrix"), 1, GL_FALSE, (GLfloat*)&projection[0]);
	
}
void makeWindmill() {
	matScaled = glm::scale(mat4(1.0f), vec3(20.0f, 20.0f, 20.0f));
	mat4 rotateMatrix = glm::rotate(glm::mat4(1.0f), radians(45.0f + rotateAngle), glm::vec3(1.0f, 0.0f, 0.0f));
	mat4 translateMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(50.0f, 70.0f, -80.0f));
	model_matrix = translateMatrix * matScaled * rotateMatrix;

	glUniformMatrix4fv(model_matrix_loc, 1, GL_FALSE, (GLfloat *)&model_matrix[0]);
	renderFan();
}
void makeLake() {
	matScaled = glm::scale(mat4(1.0f), vec3(30.0f, 30.0f, 30.0f));
	matTranslate = glm::translate(mat4(1.0f), vec3(-10.0, 25.0, 40.0f));
	mat4 xRotate = glm::rotate(glm::mat4(1.0f), radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	matRotate = glm::rotate(glm::mat4(1.0f), radians(90.0f + rotateAngle), glm::vec3(0.0f, 0.0f, 1.0f));
	model_matrix = matTranslate*xRotate *matScaled;

	setMatrices();
	glBindTexture(GL_TEXTURE_2D, fanID);
	renderDisk();
}
void makeDisk() {
	matScaled = glm::scale(mat4(1.0f), vec3(30.0f, 0.0f, 10.0f));
	matTranslate = glm::translate(mat4(1.0f), vec3(40.0, 70.0, -90.0f));
	mat4 xRotate = glm::rotate(glm::mat4(1.0f), radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	matRotate = glm::rotate(glm::mat4(1.0f), radians(90.0f + rotateAngle), glm::vec3(0.0f, 0.0f, 1.0f));
	model_matrix = matTranslate*matRotate*matScaled;

	setMatrices();
	glBindTexture(GL_TEXTURE_2D, fanID);
	renderDisk();

	matScaled = glm::scale(mat4(1.0f), vec3(30.0f, 0.0f, 10.0f));
	matTranslate = glm::translate(mat4(1.0f), vec3(40.0, 70.0, -90.0f));
	xRotate = glm::rotate(glm::mat4(1.0f), radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	matRotate = glm::rotate(glm::mat4(1.0f), radians(180.0f + rotateAngle), glm::vec3(0.0f, 0.0f, 1.0f));
	model_matrix = matTranslate * matRotate*matScaled;

	setMatrices();
	glBindTexture(GL_TEXTURE_2D, fanID);
	renderDisk();
	
	matScaled = glm::scale(mat4(1.0f), vec3(20.0f, 100.0f, 20.0f));
	matTranslate = glm::translate(mat4(1.0f), vec3(40.0, 10.0, -100.0f));

	model_matrix = matTranslate * matScaled;

	setMatrices();
	glBindTexture(GL_TEXTURE_2D, windmillID);
	renderBox();

	matScaled = glm::scale(mat4(1.0f), vec3(15.0f, 10.0f, 15.0f));
	matTranslate = glm::translate(mat4(1.0f), vec3(40.0, 70.0, -100.0f));

	model_matrix = matTranslate * matScaled;

	setMatrices();
	glBindTexture(GL_TEXTURE_2D, roofID);
	renderPyramid();
}
void makePyramid() {
	matScaled = glm::scale(mat4(1.0f), vec3(200.0f, 100.0f, 100.0f));
	matTranslate = glm::translate(mat4(1.0f), vec3(-400.0, 10.0, -800.0f));

	model_matrix = matTranslate * matScaled;

	setMatrices();
	glBindTexture(GL_TEXTURE_2D, chichenItza);
	renderPyramid();
}
void makeHouse() {
	matScaled = glm::scale(mat4(1.0f), vec3(30.0f, 30.0f, 30.0f));
	matTranslate = glm::translate(mat4(1.0f), vec3(-40.0, 10.0, 0.0f));

	model_matrix = matTranslate * matScaled;

	setMatrices();
	glBindTexture(GL_TEXTURE_2D, boxID);
	renderBox();

	matScaled = glm::scale(mat4(1.0f), vec3(20.0f, 15.0f, 20.0f));
	matTranslate = glm::translate(mat4(1.0f), vec3(-40.0, 40.0, 0.0f));

	model_matrix = matTranslate * matScaled;

	setMatrices();
	glBindTexture(GL_TEXTURE_2D, pyramidID);
	renderPyramid();
}
void makeTrees() {
	matScaled = glm::scale(mat4(1.0f), vec3(20.0f, 20.0f, 20.0f));
	matTranslate = glm::translate(mat4(1.0f), vec3(70.0, 10.0, -50.0f));

	model_matrix = matTranslate * matScaled;

	setMatrices();
	glBindTexture(GL_TEXTURE_2D, treeID);
	renderCone();

	matScaled = glm::scale(mat4(1.0f), vec3(20.0f, 20.0f, 20.0f));
	matTranslate = glm::translate(mat4(1.0f), vec3(70.0, 50.0, -50.0f));

	model_matrix = matTranslate * matScaled;

	setMatrices();
	glBindTexture(GL_TEXTURE_2D, leafID);
	renderCone();

	matScaled = glm::scale(mat4(1.0f), vec3(20.0f, 20.0f, 20.0f));
	matTranslate = glm::translate(mat4(1.0f), vec3(90.0, 10.0, 50.0f));

	model_matrix = matTranslate * matScaled;

	setMatrices();
	glBindTexture(GL_TEXTURE_2D, treeID);
	renderCone();

	matScaled = glm::scale(mat4(1.0f), vec3(20.0f, 20.0f, 20.0f));
	matTranslate = glm::translate(mat4(1.0f), vec3(90.0, 50.0, 50.0f));

	model_matrix = matTranslate * matScaled;

	setMatrices();
	glBindTexture(GL_TEXTURE_2D, leafID);
	renderCone();

	matScaled = glm::scale(mat4(1.0f), vec3(20.0f, 20.0f, 20.0f));
	matTranslate = glm::translate(mat4(1.0f), vec3(-90.0, 10.0, 60.0f));

	model_matrix = matTranslate * matScaled;

	setMatrices();
	glBindTexture(GL_TEXTURE_2D, treeID);
	renderCone();

	matScaled = glm::scale(mat4(1.0f), vec3(20.0f, 20.0f, 20.0f));
	matTranslate = glm::translate(mat4(1.0f), vec3(-90.0, 50.0, 60.0f));

	model_matrix = matTranslate * matScaled;

	setMatrices();
	glBindTexture(GL_TEXTURE_2D, leafID);
	renderCone();

	matScaled = glm::scale(mat4(1.0f), vec3(20.0f, 20.0f, 20.0f));
	matTranslate = glm::translate(mat4(1.0f), vec3(-90.0, 10.0, -60.0f));

	model_matrix = matTranslate * matScaled;

	setMatrices();
	glBindTexture(GL_TEXTURE_2D, treeID);
	renderCone();

	matScaled = glm::scale(mat4(1.0f), vec3(20.0f, 20.0f, 20.0f));
	matTranslate = glm::translate(mat4(1.0f), vec3(-90.0, 50.0, -60.0f));

	model_matrix = matTranslate * matScaled;

	setMatrices();
	glBindTexture(GL_TEXTURE_2D, leafID);
	renderCone();
}
/**************************************************************************************************************/

/*************************************************************************/
void Rotate(int n)  //the "glutTimerFunc"
{
	// implement this function!
	switch (n) {
	case 1:
		rotateAngle += speed;
		glutPostRedisplay();
		glutTimerFunc(100, Rotate, 1);
		break;
	case 2:
		viewAngle += 10.0f;
		glutPostRedisplay();
		glutTimerFunc(100, Rotate, 2);
		break;
	}

}
void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'q':case 'Q':
		exit(EXIT_SUCCESS);
		break;
	case 's':case 'S':
		speed += 50;
		if (speed >= 1000) {
			speed = 100;
		}
	}
	glutPostRedisplay();
}
/************************************************************************/
int main(int argc, char** argv) {

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA);
	glutInitWindowSize(1024, 1024);

	glutCreateWindow("Scenery");

	if (glewInit()) {
		printf("Unable to initialize GLEW ... exiting\n");
	}
	ilInit();
	Initialize();
	printf("%s\n", glGetString(GL_VERSION));
	glutKeyboardFunc(keyboard);
	glutDisplayFunc(Display);
	glutReshapeFunc(Reshape);
	glutTimerFunc(100, Rotate, 1);
	glutMainLoop();
}

/*****/



/*************/