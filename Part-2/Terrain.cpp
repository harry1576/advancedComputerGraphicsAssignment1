//  ========================================================================
//  COSC422: Computer Graphics (2018);  University of Canterbury.
//
//  FILE NAME: Terrain.cpp
//  This is part of Assignment1 files.
//
//	The program generates and loads the mesh data for a terrain floor (100 verts, 81 elems).
//  Required files:  Terrain.vert (vertex shader), Terrain.frag (fragment shader), HeightMap1.tga  (height map)
//  ========================================================================

#include <iostream>
#include <fstream>
#include <sstream>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "loadTGA.h"
using namespace std;

GLuint vaoID;
GLuint theProgram;
GLuint mvpMatrixLoc;
GLuint camVectorID;

float CDR = 3.14159265/180.0;     //Conversion from degrees to rad (required in GLM 0.9.6)
double cam_x = 0;
double cam_z = 0;

float verts[100*3];       //10x10 grid (100 vertices)
GLushort elems[81*4];       //Element array for 81 quad patches

glm::mat4 projView;

int distanceaway;
float angle = 90;

glm::vec4 cameraPos(0,15,0,0);
GLuint program;

glm::mat4 proj, view;   //Projection and view matrices
GLint WaterLevelloc;

GLuint SnowLevelloc;
GLuint heightMapSelect;

int line_mode = 0;
double waterLevelValue = 3;
double snowLevelValue =10;

//Generate vertex and element data for the terrain floor
void generateData()
{
	int indx, start;
	//verts array
	for(int i = 0; i < 10; i++)   //100 vertices on a 10x10 grid
	{
		for(int j = 0; j < 10; j++)
		{
			indx = 10*i + j;
			verts[3*indx] = 10*i - 45;		//x  varies from -45 to +45
			verts[3*indx+1] = 0;			//y  is set to 0 (ground plane)
			verts[3*indx+2] = -10*j;		//z  varies from 0 to -100
		}
	}

	//elems array
	for(int i = 0; i < 9; i++)
	{
		for(int j = 0; j < 9; j++)
		{
			indx = 9*i +j;
			start = 10*i + j;
			elems[4*indx] = start;
			elems[4*indx+1] = start+10;
			elems[4*indx+2] = start+11;
			elems[4*indx+3] = start+1;			
		}
	}
}

//Loads terrain texture
void loadTextures()
{
    GLuint texID;
    glGenTextures(1, &texID);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texID);
    loadTGA("HeightMap1.tga");

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);


    glGenTextures(2, &texID);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texID);
    loadTGA("tileable_grass.tga");
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

    glGenTextures(3, &texID);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, texID);
    loadTGA("water.tga");
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

    glGenTextures(4, &texID);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, texID);
    loadTGA("rock01.tga");
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

    glGenTextures(5, &texID);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, texID);
    loadTGA("snow.tga");
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);


    glGenTextures(6, &texID);
    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, texID);
    loadTGA("heightmap2.tga");
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

}

//Loads a shader file and returns the reference to a shader object
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

//Initialise the shader program, create and load buffer data
void initialise()
{
//--------Load terrain height map-----------
	loadTextures();
//--------Load shaders----------------------
	GLuint shaderv = loadShader(GL_VERTEX_SHADER, "Terrain.vert");
	GLuint shaderf = loadShader(GL_FRAGMENT_SHADER, "Terrain.frag");
    GLuint shaderc = loadShader(GL_TESS_CONTROL_SHADER, "Terrain.cont");
    GLuint shadere = loadShader(GL_TESS_EVALUATION_SHADER, "Terrain.eval");
    GLuint shaderg = loadShader(GL_GEOMETRY_SHADER, "Terrain.geo");


    program = glCreateProgram();
	glAttachShader(program, shaderv);
	glAttachShader(program, shaderf);
    glAttachShader(program, shaderc);
    glAttachShader(program, shadere);
     glAttachShader(program, shaderg);
	glLinkProgram(program);

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
	glUseProgram(program);

	mvpMatrixLoc = glGetUniformLocation(program, "mvpMatrix");
	GLuint texLoc = glGetUniformLocation(program, "heightMap");
    camVectorID = glGetUniformLocation(program, "camVector");
    GLuint texLoc2 = glGetUniformLocation(program, "grass");

    GLuint texLoc3 = glGetUniformLocation(program, "water");
    GLuint texLoc4 = glGetUniformLocation(program, "rock");
    GLuint texLoc5 = glGetUniformLocation(program, "snow");
    WaterLevelloc = glGetUniformLocation(program, "waterLevel");
    SnowLevelloc = glGetUniformLocation(program, "snowLevel");

    heightMapSelect = glGetUniformLocation(program, "mapSelect");



    GLuint texLoc6 = glGetUniformLocation(program, "heightMap2");

    glUniform1i(texLoc, 0);

    glUniform1i(texLoc2, 1);

    glUniform1i(texLoc3, 2);
    glUniform1i(texLoc4, 3);
    glUniform1i(texLoc5, 4);
    glUniform1i(texLoc6, 5);



    glUniform1f(WaterLevelloc, waterLevelValue);
    glUniform1f(SnowLevelloc, snowLevelValue);

    glUniform1i(heightMapSelect, 1);








//--------Compute matrices----------------------

    proj = glm::perspective(30.0f*CDR, 1.25f, 20.0f, 500.0f);  //perspective projection matrix
    view = glm::lookAt(glm::vec3(cam_x, 15.0 , cam_z), glm::vec3(cam_x + cos(-angle * (M_PI/180)), 15.0, cam_z + sin(-angle * (M_PI/180))), glm::vec3(0.0, 1.0, 0.0)); //view matrix
    projView = proj * view;  //Product (mvp) matrix




//---------Load buffer data-----------------------
	generateData();

    GLuint vboID[3];
	glGenVertexArrays(1, &vaoID);
    glBindVertexArray(vaoID);

    glGenBuffers(3, vboID);

    glBindBuffer(GL_ARRAY_BUFFER, vboID[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);  // Vertex position

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboID[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elems), elems, GL_STATIC_DRAW);


    glBindVertexArray(0);

    glPatchParameteri(GL_PATCH_VERTICES, 4);

    glEnable(GL_DEPTH_TEST);
   // glEnable(GL_CULL_FACE);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void display()
{
	glUniformMatrix4fv(mvpMatrixLoc, 1, GL_FALSE, &projView[0][0]);
    glUseProgram(program);

    glUniform4f(camVectorID, cameraPos.x,cameraPos.y,cameraPos.z,1);


    proj = glm::perspective(30.0f*CDR, 1.25f, 20.0f, 500.0f);  //perspective projection matrix
    view = glm::lookAt(glm::vec3(cam_x, 15.0 , cam_z), glm::vec3(cam_x + cos(-angle * (M_PI/180)), 15.0, cam_z + sin(-angle * (M_PI/180))), glm::vec3(0.0, 1.0, 0.0)); //view matrix

    cameraPos.x = cam_x;
    cameraPos.y = 15.0;
    cameraPos.z = cam_z;


    projView = proj * view;  //Product (mvp) matrix

    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glBindVertexArray(vaoID);
    glDrawElements(GL_PATCHES, 81*4, GL_UNSIGNED_SHORT, NULL);
    glFlush();
}


void special(int key, int x, int y)
{
   if(key == GLUT_KEY_UP )
   {
       cam_x += cos((M_PI/180)*-angle) ;
       cam_z += sin((M_PI/180)*-angle) ;
       distanceaway = pow(pow(cam_x,2) + pow(cam_z,2),0.5) ;
       //cout << "X " << cam_x << endl;
      // cout << "Z " << cam_z << endl;
       //cout << "View " << view[0][0] << endl;

       glutPostRedisplay();
   }
   else if(key == GLUT_KEY_DOWN )
   {
       cam_x += cos(-(M_PI/180)*(angle - 180)) ;
       cam_z += sin(-(M_PI/180)*(angle - 180)) ;
       distanceaway = pow(pow(cam_x,2) + pow(cam_z,2),0.5) ;
       //cout << "X " << cam_x << endl;
       //cout << "Z " << cam_z << endl;

       glutPostRedisplay();
   }
   else if(key == GLUT_KEY_RIGHT)
   {
       angle -= 1;
       //cout << "Angle " << angle << endl;

       glutPostRedisplay();
   }
   else if(key == GLUT_KEY_LEFT)
   {
       angle += 1;
      // cout << "Angle " << angle << endl;

       glutPostRedisplay();
   }
}


void keyBoard (unsigned char key, int x, int y)
{

    if (key == 'w' && line_mode == 0)
    {
        line_mode = 1;
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glutPostRedisplay();

    }
    else if (key == 'w' && line_mode == 1)
    {
        line_mode = 0;
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glutPostRedisplay();

    }
    if (key == 'o' && waterLevelValue < 3.9)
    {
        waterLevelValue += 0.1;
        glUniform1f(WaterLevelloc, waterLevelValue);
        glutPostRedisplay();


    }
    else if (key == 'p' && waterLevelValue > -0.1)
    {
        waterLevelValue -= 0.1;
        glUniform1f(WaterLevelloc, waterLevelValue);
        glutPostRedisplay();
    }
    if (key == 'k' && snowLevelValue < 12)
    {
        snowLevelValue += 0.1;
        glUniform1f(SnowLevelloc, snowLevelValue);
        glutPostRedisplay();


    }
    else if (key == 'l'  && snowLevelValue > 9)
    {
        snowLevelValue -= 0.1;
        glUniform1f(SnowLevelloc, snowLevelValue);
        //cout << "Angle " << snowLevelValue << endl;

        glutPostRedisplay();
    }
    else if (key == '1'  )
    {
        glUniform1i(heightMapSelect, 1);


        glutPostRedisplay();

    }
    else if (key == '2' )
    {
        glUniform1i(heightMapSelect, 0);


        glutPostRedisplay();
    }




}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_DEPTH);
	glutInitWindowSize(1000, 800);
	glutCreateWindow("Terrain");
	glutInitContextVersion (4, 2);
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
    glutSpecialFunc(special);
    glutKeyboardFunc(keyBoard);

	glutDisplayFunc(display);
	glutMainLoop();
}

