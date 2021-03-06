#include "stdafx.h"
#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/wglew.h>
#include <GLM/glm.hpp>

using namespace std;

GLuint g_programID;
GLuint Buffers[3];
GLuint matTrans, matRot;
GLint colorAttrib;
float r, g, b;

vector<float> c_vector = {
	0.1f, 0.1f,0.1f,
	-0.1f, 0.1f, 0.1f,
	-0.1f, -0.1f, 0.1f,
	0.1f, -0.1f, 0.1f,
	0.1f, -0.1f, -0.1f,
	0.1f, 0.1f,-0.1f,
	-0.1f, 0.1f,-0.1f,
	-0.1f,-0.1f,-0.1f,
}
;//점 저장
vector<unsigned int> i_vector = { //element로
	0, 1, 5, 1, 6, 5,//위
	0, 5, 4, 0, 4, 3,//오
	5, 6, 7, 5, 7, 4,//앞
	1, 6, 2, 6, 7, 2,//왼
	0, 3, 2, 0, 2, 1,//뒤
	2, 7, 3, 3, 7, 4//밑
};

vector<float> col_vector = {
	0.5f, 0.0f, 0.0f,
	0.5f, 0.0f, 0.1f,
	0.5f, 0.0f, 0.2f,
	0.4f, 0.0f, 0.3f,
	0.4f, 0.0f, 0.4f,
	0.4f, 0.0f, 0.5f,
	0.3f, 0.0f, 0.6f,
	0.3f, 0.0f, 0.7f
};

GLfloat transR[]{
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 1, 0,
	0, 0, 0, 1
};
GLfloat transL[]{
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 1, 0,
	0, 0, 0, 1
};

GLuint LoadShaders(const char* vertex_file_path, const char* fragment_file_path)
{
	//create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	GLint Result = GL_FALSE;
	int InfoLogLength;

	//Read the vertex shader code from the file
	string VertexShaderCode;
	ifstream VertexShaderStream(vertex_file_path, ios::in);
	if (VertexShaderStream.is_open())
	{
		string Line = "";
		while (getline(VertexShaderStream, Line))
			VertexShaderCode += "\n" + Line;
		VertexShaderStream.close();
	}

	//Compile Vertex Shader
	printf("Compiling shader : %s\n", vertex_file_path);
	char const* VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
	glCompileShader(VertexShaderID);

	//Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);

	if (InfoLogLength != 0)
	{
		vector<char> VertexShaderErrorMessage(InfoLogLength);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		fprintf(stdout, "%s\n", &VertexShaderErrorMessage[0]);
	}

	//Read the fragment shader code from the file
	string FragmentShaderCode;
	ifstream FragmentShaderStream(fragment_file_path, ios::in);
	if (FragmentShaderStream.is_open())
	{
		string Line = "";
		while (getline(FragmentShaderStream, Line))
			FragmentShaderCode += "\n" + Line;
		FragmentShaderStream.close();
	}

	//Compile Fragment Shader
	printf("Compiling shader : %s\n", fragment_file_path);
	char const* FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
	glCompileShader(FragmentShaderID);

	//Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength != 0)
	{
		vector<char> FragmentShaderErrorMessage(InfoLogLength);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		fprintf(stdout, "%s\n", &FragmentShaderErrorMessage[0]);
	}
	//Link the program
	fprintf(stdout, "Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}


void renderScene(void)
{
	//Clear all pixels
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//set Matrix
	glUniformMatrix4fv(matTrans, 1, GL_FALSE, transR);
	glDrawElements(GL_TRIANGLES,i_vector.size(), GL_UNSIGNED_INT, 0); // draw left

	//Double buffer
	glutSwapBuffers();
}

void setNewColor() {
	r = (rand() % 100) / 99.0f;
	g = (rand() % 100) / 99.0f;
	b = (rand() % 100) / 99.0f;
}

void myMouseFunc(int btn, int state, int x, int y) {
	if (state == GLUT_DOWN && btn == GLUT_LEFT_BUTTON) {
		float nx = 2.0f * (float)x / (float)479 - 1.0f; //mapping to world coordinate
		float ny = -2.0f * (float)y / (float)479 + 1.0f;

		// look at Matrix
		glm::vec3 uz = glm::normalize( glm::vec3(nx, ny, 0));
		glm::vec3 p = glm::vec3(0, 0, 1);
		glm::vec3 ux = glm::cross(uz, p); 
		glm::vec3 uy = glm::cross(ux, uz);

		glm::vec4 M[4];
		M[0] = glm::vec4(ux, 0);
		M[1] = glm::vec4(uy, 0);
		M[2] = glm::vec4(uz, 0);
		M[3] = glm::vec4(0, 0, 0, 1);

		glUniformMatrix4fv(matRot, 1, GL_FALSE, &M[0][0]);

		glutPostRedisplay();//re-draw
	}
}

void myKeyFunc(unsigned char key, int x, int y) {
	switch (key) {
	case 'q': //move the left cube coordinate
		transL[13] += 0.1f;
		break;
	case 'w':
		transL[13] -= 0.1f;
		break;
	case 'a':
		transL[14] += 0.1f;
		break;
	case 'z':
		transL[14] -= 0.1f;
		break;
	case 'e': //move the right cube y coordinate
		transR[13] += 0.1f;
		break;
	case 'r':
		transR[13] -= 0.1f;
		break;
	case 's':
		transR[14] += 0.1f;
		break;
	case 'x':
		transR[14] -= 0.1f;
		break;
	}
	glutPostRedisplay();// re-draw
}

void init()
{
	GLenum res = glewInit();
	if(res != GLEW_OK)
		fprintf(stderr, "Error: '%s' \n", glewGetErrorString(res));

	glClearColor(1.0, 1.0, 1.0, 1.0);
	glLineWidth(30.0f);
	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_DEPTH_FUNC);
	glDepthFunc(GL_LESS);
	glDepthRange(0.5, 1.0);
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);

	glutInitWindowPosition(200, 200);
	glutInitWindowSize(480, 480);
	glutCreateWindow("hello OpenGL");
	init();

	g_programID = LoadShaders("VertexShader.txt", "FragmentShader.txt");
	glUseProgram(g_programID);
	matTrans = glGetUniformLocation(g_programID, "T");
	matRot = glGetUniformLocation(g_programID, "R");

	//1.
	//Generate VAO
	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	//2.
	//Generate VBO
	// index
	glGenBuffers(3, Buffers);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Buffers[0]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(float) *i_vector.size(), i_vector.data(), GL_DYNAMIC_DRAW);

	// coordinate
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[1]); 
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * c_vector.size(), c_vector.data(), GL_DYNAMIC_DRAW); 
	GLint posAttrib = glGetAttribLocation(g_programID, "pos");
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);
	glEnableVertexAttribArray(posAttrib); // active

	// color
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * col_vector.size(), col_vector.data(), GL_DYNAMIC_DRAW); 
	colorAttrib = glGetAttribLocation(g_programID, "a_Color");
	glVertexAttribPointer(colorAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);
	glEnableVertexAttribArray(colorAttrib); // active

	//Set Function
	glutMouseFunc(myMouseFunc);
	glutKeyboardFunc(myKeyFunc);
	glutDisplayFunc(renderScene);

	//enter GLUT event processing cycle
	glutMainLoop();

	glDeleteVertexArrays(1, &VertexArrayID);
	return 1;
}