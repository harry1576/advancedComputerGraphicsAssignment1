//  ========================================================================
//  COSC422: Advanced Computer Graphics;  University of Canterbury.
//
//  FILE NAME: CubePatches.cpp
//  See Ex-06.pdf for details.
//
//	The program generates and loads the mesh data for a cube (8 verts, 24 elems).
//  Requires files  CubePatches.h, CubePatches.vert, CubePatches.frag
//  ========================================================================

#include <iostream>
#include <fstream>
#include <sstream>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "CubePatches.h"
#include <math.h>

using namespace std;

GLuint vboID[5];
GLuint vboID2[5];
GLuint GridVbo;

GLuint vaoID;
GLuint vaoID2;
GLuint theProgram;
GLuint matrixLoc;
float angle = -90.0;
glm::mat4 projView;
float CDR = 3.14159265/180.0;   //Conversion from degrees to radians (required in GLM 0.9.6 and later versions)
GLuint program;
GLuint programLines;

bool line_mode = 0;
float timepassed = 0;
int distanceaway = 0;

int explode = 0;

GLuint loadShader(GLenum shaderType, string filename)
{
	ifstream shaderFile(filename.c_str());
	if(!shaderFile.good()) cout << "Error opening shader file." << endl;
	stringstream shaderData;
	shaderData << shaderFile.rdbuf();
	shaderFile.close();
	string shaderStr = shaderData.str();
	const char* shaderTxt = shaderStr.c_str();

	GLuint shader = glCreateShader(shaderType);
	glShaderSource(shader, 1, &shaderTxt, NULL);
	glCompileShader(shader);
	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint infoLogLength;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
		GLchar *strInfoLog = new GLchar[infoLogLength + 1];
		glGetShaderInfoLog(shader, infoLogLength, NULL, strInfoLog);
		const char *strShaderType = NULL;
		cerr <<  "Compile failure in shader: " << strInfoLog << endl;
		delete[] strInfoLog;
	}
	return shader;
}

void update(int value)
{
    //angle++;
    if(explode == 1){
        timepassed ++;
        glDisable(GL_CULL_FACE);

    }

    glutTimerFunc(25, update, 0);
    glutPostRedisplay();
}
void initialise()
{
	GLuint shaderv = loadShader(GL_VERTEX_SHADER, "CubePatches.vert");
	GLuint shaderf = loadShader(GL_FRAGMENT_SHADER, "CubePatches.frag");
    GLuint shaderc = loadShader(GL_TESS_CONTROL_SHADER, "CubePatches.cont");
    GLuint shadere = loadShader(GL_TESS_EVALUATION_SHADER, "CubePatches.eval");
    GLuint shaderg = loadShader(GL_GEOMETRY_SHADER, "TeaPot.geo");


    GLuint shaderLinesv = loadShader(GL_VERTEX_SHADER, "floorLines.vert");
    GLuint shaderLinesf = loadShader(GL_FRAGMENT_SHADER, "floorLines.frag");

    program = glCreateProgram();
	glAttachShader(program, shaderv);
	glAttachShader(program, shaderf);
    glAttachShader(program, shaderc);
    glAttachShader(program, shadere);
    glAttachShader(program, shaderg);
    glLinkProgram(program);

    programLines = glCreateProgram();
    glAttachShader(programLines, shaderLinesv);
    glAttachShader(programLines, shaderLinesf);
    glLinkProgram(programLines);

	GLint status;
	glGetProgramiv (program, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint infoLogLength;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);
		GLchar *strInfoLog = new GLchar[infoLogLength + 1];
		glGetProgramInfoLog(program, infoLogLength, NULL, strInfoLog);
		fprintf(stderr, "Linker failure: %s\n", strInfoLog);
		delete[] strInfoLog;
    }


    glGenVertexArrays(1, &vaoID);
    glBindVertexArray(vaoID);

    glUseProgram(program);

    glGenBuffers(2, vboID);
    glBindBuffer(GL_ARRAY_BUFFER, vboID[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);  // Vertex position

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboID[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elems), elems, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);


    glUseProgram(programLines);
    glGenVertexArrays(1, &vaoID2);
    glBindVertexArray(vaoID2);
    glGenBuffers(1, vboID2);
    glBindBuffer(GL_ARRAY_BUFFER, vboID2[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Lineverts), Lineverts, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);  // Vertex position
    glBindVertexArray(0);


    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);
   //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glutTimerFunc(50, update, 0);

}



void display()
{

    glm::mat4 proj, view, mvMatrix;

    glm::mat4 matrix = glm::mat4(1.0);
    //matrix = glm::rotate(matrix, angle*CDR, glm::vec3(0.0, 1.0, 0.0));  //rotation matrix
    //matrix = glm::rotate(matrix, -90.0f, glm::vec3(0.0, 1.0, 0.0));  //rotation matrix

    proj = glm::perspective(45.0f*CDR, 1.0f, 10.0f, 1000.0f);  //perspective projection matrix
    view = glm::lookAt(glm::vec3(0.0, 15.0, -100.0 + distanceaway), glm::vec3(0.0, 15, 0.0), glm::vec3(0.0, 1.0, 0.0)); //view matrix

    mvMatrix = view * matrix;


    projView = proj*view;  //Product matrix

    glm::mat4 prodMatrix = projView * matrix;        //Model-view-proj matrix


    glUseProgram(program);

    GLuint Matrix2ID = glGetUniformLocation(program, "mvpMatrix");
    glUniformMatrix4fv(Matrix2ID, 1, GL_FALSE, &prodMatrix[0][0]);

    GLuint timeShaderHandle = glGetUniformLocation(program,"time");
    glUniform1f(timeShaderHandle, timepassed);

    GLuint MatrixID = glGetUniformLocation(program, "mvMatrixl");
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvMatrix[0][0]);


    glUseProgram(programLines);

    GLuint Matrix3ID = glGetUniformLocation(programLines, "mvpMatrix");
    glUniformMatrix4fv(Matrix3ID, 1, GL_FALSE, &prodMatrix[0][0]);



   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT|GL_ACCUM_BUFFER_BIT);


    glUseProgram(programLines);
    glBindVertexArray(vaoID2);
    glBindBuffer(GL_ARRAY_BUFFER, vboID2[0]);
    glDrawArrays(GL_LINES,0,28);

    glUseProgram(program);
    glBindVertexArray(vaoID);
    glBindBuffer(GL_ARRAY_BUFFER, vboID[1]);
    glPatchParameteri(GL_PATCH_VERTICES, 16);
    glDrawElements(GL_PATCHES, 2048, GL_UNSIGNED_SHORT, NULL);


    glutSwapBuffers();
    //glFlush();
}

void keyBoard (unsigned char key, int x, int y)
{

    if (key == 'w' && line_mode == 0)
    {
        line_mode = 1;
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        //glutPostRedisplay();

    }
    else if (key == 'w' && line_mode == 1)
    {
        line_mode = 0;
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        //glutPostRedisplay();

    }
    else if (key == 32) { // space key
        explode = 1;
    }

}


void special(int key, int x, int y)
{
   if(key == GLUT_KEY_UP && distanceaway < 55)
   {
       distanceaway += 1;
       //glutPostRedisplay();
   }
   else if(key == GLUT_KEY_DOWN && distanceaway > -100)
   {
       distanceaway -= 1;
       //glutPostRedisplay();
   }
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB|GLUT_DEPTH);
    glutInitWindowSize(800, 800);
    glutCreateWindow("Bezier Surface Modelling");
    glutInitContextVersion (4, 2 );
	glutInitContextProfile ( GLUT_CORE_PROFILE );

	if(glewInit() == GLEW_OK)
	{
		cout << "GLEW initialization successful! " << endl;
		cout << " Using GLEW version " << glewGetString(GLEW_VERSION) << endl;
	}
	else
	{
		cerr << "Unable to initialize GLEW  ...exiting." << endl;
		exit(EXIT_FAILURE);
	}

	initialise();
    glutDisplayFunc(display);
    glutSpecialFunc(special);
    glutKeyboardFunc(keyBoard);


	glutMainLoop();
}

