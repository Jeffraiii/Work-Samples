/*
  CSCI 420 Computer Graphics, USC
  Assignment 1: Height Fields
  C++ starter code

  Student username: tianmul
*/

#include "basicPipelineProgram.h"
#include "texturePipelineProgram.h"
#include "openGLMatrix.h"
#include "imageIO.h"
#include "openGLHeader.h"
#include "glutHeader.h"
#include <vector>
#include <iostream>
#include <cstring>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glm/gtc/type_ptr.hpp>

#if defined(WIN32) || defined(_WIN32)
  #ifdef _DEBUG
    #pragma comment(lib, "glew32d.lib")
  #else
    #pragma comment(lib, "glew32.lib")
  #endif
#endif

#if defined(WIN32) || defined(_WIN32)
  char shaderBasePath[1024] = SHADER_BASE_PATH;
#else
  char shaderBasePath[1024] = "../openGLHelper-starterCode";
#endif

using namespace std;

int mousePos[2]; // x,y coordinate of the mouse position

int leftMouseButton = 0; // 1 if pressed, 0 if not 
int middleMouseButton = 0; // 1 if pressed, 0 if not
int rightMouseButton = 0; // 1 if pressed, 0 if not

typedef enum { ROTATE, TRANSLATE, SCALE } CONTROL_STATE;
CONTROL_STATE controlState = ROTATE;

//enum for display mode
typedef enum { PTS, LINES, TRIANGLES } DISPLAY_MODE;
DISPLAY_MODE displayMode = PTS;

// state of the world
float landRotate[3] = { 0.0f, 0.0f, 0.0f };
float landTranslate[3] = { 0.0f, 0.0f, 0.0f };
float landScale[3] = { 1.0f, 1.0f, 1.0f };

int windowWidth = 1280;
int windowHeight = 720;
char windowTitle[512] = "CSCI 420 homework I";

ImageIO * heightmapImage;

OpenGLMatrix openGLMatrix;
BasicPipelineProgram pipelineProgram;
TexturePipelineProgram texPipelineProgram;
GLint h_modelViewMatrix, h_projectionMatrix;
GLuint vVao, groundVAO; //global VAOs for each display mode
GLuint texHandle;
GLuint vbo, groundVBO;
//global vectors to store vertex data
std::vector<float> positionVec;
std::vector<glm::vec3> tangentVec;
std::vector<float> colorVec;
std::vector<glm::vec3> normalVec;
std::vector<glm::vec3> splinePositionVec;
std::vector<float> groundPosVec;
std::vector<float> groundUVVec;


int sscount = 0;

int cameraTangentIndex = 0;
int cameraPositionIndex = 0;
int cameraNormalIndex = 0;

// represents one control point along the spline 
struct Point
{
	double x;
	double y;
	double z;
};

// spline struct 
// contains how many control points the spline has, and an array of control points 
struct Spline
{
	int numControlPoints;
	Point * points;
};

// the spline array 
Spline * splines;
// total number of splines 
int numSplines;

int loadSplines(char * argv)
{
	char * cName = (char *)malloc(128 * sizeof(char));
	FILE * fileList;
	FILE * fileSpline;
	int iType, i = 0, j, iLength;

	// load the track file 
	fileList = fopen(argv, "r");
	if (fileList == NULL)
	{
		printf("can't open file\n");
		exit(1);
	}

	// stores the number of splines in a global variable 
	fscanf(fileList, "%d", &numSplines);

	splines = (Spline*)malloc(numSplines * sizeof(Spline));

	// reads through the spline files 
	for (j = 0; j < numSplines; j++)
	{
		i = 0;
		fscanf(fileList, "%s", cName);
		fileSpline = fopen(cName, "r");

		if (fileSpline == NULL)
		{
			printf("can't open file\n");
			exit(1);
		}

		// gets length for spline file
		fscanf(fileSpline, "%d %d", &iLength, &iType);

		// allocate memory for all the points
		splines[j].points = (Point *)malloc(iLength * sizeof(Point));
		splines[j].numControlPoints = iLength;

		// saves the data to the struct
		while (fscanf(fileSpline, "%lf %lf %lf",
			&splines[j].points[i].x,
			&splines[j].points[i].y,
			&splines[j].points[i].z) != EOF)
		{
			i++;
		}
	}

	free(cName);

	return 0;
}

//glm::vec3 computeSplinePosition(float u) {
//
//}
//
//glm::vec3 computeSplineTangent(float u) {
//
//}

int initTexture(const char * imageFilename, GLuint textureHandle)
{
	// read the texture image
	ImageIO img;
	ImageIO::fileFormatType imgFormat;
	ImageIO::errorType err = img.load(imageFilename, &imgFormat);

	if (err != ImageIO::OK)
	{
		printf("Loading texture from %s failed.\n", imageFilename);
		return -1;
	}

	// check that the number of bytes is a multiple of 4
	if (img.getWidth() * img.getBytesPerPixel() % 4)
	{
		printf("Error (%s): The width*numChannels in the loaded image must be a multiple of 4.\n", imageFilename);
		return -1;
	}

	// allocate space for an array of pixels
	int width = img.getWidth();
	int height = img.getHeight();
	unsigned char * pixelsRGBA = new unsigned char[4 * width * height]; // we will use 4 bytes per pixel, i.e., RGBA

	// fill the pixelsRGBA array with the image pixels
	memset(pixelsRGBA, 0, 4 * width * height); // set all bytes to 0
	for (int h = 0; h < height; h++)
		for (int w = 0; w < width; w++)
		{
			// assign some default byte values (for the case where img.getBytesPerPixel() < 4)
			pixelsRGBA[4 * (h * width + w) + 0] = 0; // red
			pixelsRGBA[4 * (h * width + w) + 1] = 0; // green
			pixelsRGBA[4 * (h * width + w) + 2] = 0; // blue
			pixelsRGBA[4 * (h * width + w) + 3] = 255; // alpha channel; fully opaque

			// set the RGBA channels, based on the loaded image
			int numChannels = img.getBytesPerPixel();
			for (int c = 0; c < numChannels; c++) // only set as many channels as are available in the loaded image; the rest get the default value
				pixelsRGBA[4 * (h * width + w) + c] = img.getPixel(w, h, c);
		}

	// bind the texture
	glBindTexture(GL_TEXTURE_2D, textureHandle);

	// initialize the texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixelsRGBA);

	// generate the mipmaps for this texture
	glGenerateMipmap(GL_TEXTURE_2D);

	// set the texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// query support for anisotropic texture filtering
	GLfloat fLargest;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &fLargest);
	printf("Max available anisotropic samples: %f\n", fLargest);
	// set anisotropic texture filtering
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 0.5f * fLargest);

	// query for any errors
	GLenum errCode = glGetError();
	if (errCode != 0)
	{
		printf("Texture initialization error. Error code: %d.\n", errCode);
		return -1;
	}

	// de-allocate the pixel array -- it is no longer needed
	delete[] pixelsRGBA;

	return 0;
}

void loadGroundBufferData() {
	groundPosVec.push_back(-100);
	groundPosVec.push_back(100);
	groundPosVec.push_back(-100);
	groundUVVec.push_back(0);
	groundUVVec.push_back(1);

	groundPosVec.push_back(100);
	groundPosVec.push_back(100);
	groundPosVec.push_back(-100);
	groundUVVec.push_back(1);
	groundUVVec.push_back(1);

	groundPosVec.push_back(-100);
	groundPosVec.push_back(100);
	groundPosVec.push_back(100);
	groundUVVec.push_back(0);
	groundUVVec.push_back(0);

	groundPosVec.push_back(100);
	groundPosVec.push_back(100);
	groundPosVec.push_back(-100);
	groundUVVec.push_back(1);
	groundUVVec.push_back(1);

	groundPosVec.push_back(-100);
	groundPosVec.push_back(100);
	groundPosVec.push_back(100);
	groundUVVec.push_back(0);
	groundUVVec.push_back(0);

	groundPosVec.push_back(100);
	groundPosVec.push_back(100);
	groundPosVec.push_back(100);
	groundUVVec.push_back(1);
	groundUVVec.push_back(0);
}

//loads data for points mode
void loadTrackBufferData() {
	float alpha = 0.1f;
	glm::vec3 v;
	v.x = 1;
	v.y = 1;
	v.z = 0;
	float s = 0.5f;
	glm::mat4x4 basis = glm::mat4x4(-s, 2 - s, s - 2, s,
		2 * s, s - 3, 3 - 2 * s, -s,
		-s, 0, s, 0,
		0, 1, 0, 0);
	float u = 0;
	glm::mat4x3 control;
	glm::mat4x3 combined;
	glm::vec3 position;
	glm::vec3 tangent;
	Point p0;
	p0.x = splines[0].points[0].x + (splines[0].points[0].x - splines[0].points[1].x);
	p0.y = splines[0].points[0].y + (splines[0].points[0].y - splines[0].points[1].y);
	p0.z = splines[0].points[0].z + (splines[0].points[0].z - splines[0].points[1].z);
	int i = 0;
	for (u = 0; u < 1; u += 0.001) {
		control = glm::mat4x3((const float)p0.x, (const float)p0.y, (const float)p0.z,
			(const float)splines[0].points[i].x, (const float)splines[0].points[i].y, (const float)splines[0].points[i].z,
			(const float)splines[0].points[i + 1].x, (const float)splines[0].points[i + 1].y, (const float)splines[0].points[i + 1].z,
			(const float)splines[0].points[i + 2].x, (const float)splines[0].points[i + 2].y, (const float)splines[0].points[i + 2].z);
		combined = control * basis;
		position = combined * glm::vec4(u*u*u, u*u, u, 1);
		tangent = combined * glm::vec4(3 * u * u, 2 * u, 1, 0);
		glm::vec3 n0 = glm::normalize(glm::cross(tangent, v));
		glm::vec3 b0 = glm::normalize(glm::cross(tangent, n0));
		glm::vec3 v0 = position + alpha * (-n0 + b0);
		glm::vec3 v1 = position + alpha * (n0 + b0);
		glm::vec3 v2 = position + alpha * (n0 - b0);
		glm::vec3 v3 = position + alpha * (-n0 - b0);

		normalVec.push_back(n0);
		splinePositionVec.push_back(position);
		tangentVec.push_back(tangent);
		u += 0.001;

		control = glm::mat4x3((const float)p0.x, (const float)p0.y, (const float)p0.z,
			(const float)splines[0].points[i].x, (const float)splines[0].points[i].y, (const float)splines[0].points[i].z,
			(const float)splines[0].points[i + 1].x, (const float)splines[0].points[i + 1].y, (const float)splines[0].points[i + 1].z,
			(const float)splines[0].points[i + 2].x, (const float)splines[0].points[i + 2].y, (const float)splines[0].points[i + 2].z);
		combined = control * basis;
		position = combined * glm::vec4(u*u*u, u*u, u, 1);
		tangent = combined * glm::vec4(3 * u * u, 2 * u, 1, 0);
		glm::vec3 n1 = glm::normalize(glm::cross(tangent, v));
		glm::vec3 b1 = glm::normalize(glm::cross(tangent, n1));
		glm::vec3 v4 = position + alpha * (-n1 + b1);
		glm::vec3 v5 = position + alpha * (n1 + b1);
		glm::vec3 v6 = position + alpha * (n1 - b1);
		glm::vec3 v7 = position + alpha * (-n1 - b1);

		v0 += 0.5f * b0;
		v1 += 0.5f * b0;
		v2 += 0.5f * b0;
		v3 += 0.5f * b0;
		v4 += 0.5f * b1;
		v5 += 0.5f * b1;
		v6 += 0.5f * b1;
		v7 += 0.5f * b1;

		//right face
		positionVec.push_back(v1.x);
		positionVec.push_back(v1.y);
		positionVec.push_back(v1.z);
		positionVec.push_back(v0.x);
		positionVec.push_back(v0.y);
		positionVec.push_back(v0.z);
		positionVec.push_back(v4.x);
		positionVec.push_back(v4.y);
		positionVec.push_back(v4.z);

		colorVec.push_back(b0.x);
		colorVec.push_back(b0.y);
		colorVec.push_back(b0.z);
		colorVec.push_back(1);
		colorVec.push_back(b0.x);
		colorVec.push_back(b0.y);
		colorVec.push_back(b0.z);
		colorVec.push_back(1);
		colorVec.push_back(b1.x);
		colorVec.push_back(b1.y);
		colorVec.push_back(b1.z);
		colorVec.push_back(1);


		positionVec.push_back(v1.x);
		positionVec.push_back(v1.y);
		positionVec.push_back(v1.z);
		positionVec.push_back(v4.x);
		positionVec.push_back(v4.y);
		positionVec.push_back(v4.z);
		positionVec.push_back(v5.x);
		positionVec.push_back(v5.y);
		positionVec.push_back(v5.z);

		colorVec.push_back(b0.x);
		colorVec.push_back(b0.y);
		colorVec.push_back(b0.z);
		colorVec.push_back(1);
		colorVec.push_back(b1.x);
		colorVec.push_back(b1.y);
		colorVec.push_back(b1.z);
		colorVec.push_back(1);
		colorVec.push_back(b1.x);
		colorVec.push_back(b1.y);
		colorVec.push_back(b1.z);
		colorVec.push_back(1);


		//top face
		positionVec.push_back(v2.x);
		positionVec.push_back(v2.y);
		positionVec.push_back(v2.z);
		positionVec.push_back(v1.x);
		positionVec.push_back(v1.y);
		positionVec.push_back(v1.z);
		positionVec.push_back(v5.x);
		positionVec.push_back(v5.y);
		positionVec.push_back(v5.z);

		colorVec.push_back(n0.x);
		colorVec.push_back(n0.y);
		colorVec.push_back(n0.z);
		colorVec.push_back(1);
		colorVec.push_back(n0.x);
		colorVec.push_back(n0.y);
		colorVec.push_back(n0.z);
		colorVec.push_back(1);
		colorVec.push_back(n1.x);
		colorVec.push_back(n1.y);
		colorVec.push_back(n1.z);
		colorVec.push_back(1);

		positionVec.push_back(v2.x);
		positionVec.push_back(v2.y);
		positionVec.push_back(v2.z);
		positionVec.push_back(v5.x);
		positionVec.push_back(v5.y);
		positionVec.push_back(v5.z);
		positionVec.push_back(v6.x);
		positionVec.push_back(v6.y);
		positionVec.push_back(v6.z);

		colorVec.push_back(n0.x);
		colorVec.push_back(n0.y);
		colorVec.push_back(n0.z);
		colorVec.push_back(1);
		colorVec.push_back(n1.x);
		colorVec.push_back(n1.y);
		colorVec.push_back(n1.z);
		colorVec.push_back(1);
		colorVec.push_back(n1.x);
		colorVec.push_back(n1.y);
		colorVec.push_back(n1.z);
		colorVec.push_back(1);

		//left face
		positionVec.push_back(v3.x);
		positionVec.push_back(v3.y);
		positionVec.push_back(v3.z);
		positionVec.push_back(v2.x);
		positionVec.push_back(v2.y);
		positionVec.push_back(v2.z);
		positionVec.push_back(v6.x);
		positionVec.push_back(v6.y);
		positionVec.push_back(v6.z);

		colorVec.push_back(-b0.x);
		colorVec.push_back(-b0.y);
		colorVec.push_back(-b0.z);
		colorVec.push_back(1);
		colorVec.push_back(-b0.x);
		colorVec.push_back(-b0.y);
		colorVec.push_back(-b0.z);
		colorVec.push_back(1);
		colorVec.push_back(-b1.x);
		colorVec.push_back(-b1.y);
		colorVec.push_back(-b1.z);
		colorVec.push_back(1);

		positionVec.push_back(v3.x);
		positionVec.push_back(v3.y);
		positionVec.push_back(v3.z);
		positionVec.push_back(v6.x);
		positionVec.push_back(v6.y);
		positionVec.push_back(v6.z);
		positionVec.push_back(v7.x);
		positionVec.push_back(v7.y);
		positionVec.push_back(v7.z);

		colorVec.push_back(-b0.x);
		colorVec.push_back(-b0.y);
		colorVec.push_back(-b0.z);
		colorVec.push_back(1);
		colorVec.push_back(-b1.x);
		colorVec.push_back(-b1.y);
		colorVec.push_back(-b1.z);
		colorVec.push_back(1);
		colorVec.push_back(-b1.x);
		colorVec.push_back(-b1.y);
		colorVec.push_back(-b1.z);
		colorVec.push_back(1);
		//bottom face
		positionVec.push_back(v0.x);
		positionVec.push_back(v0.y);
		positionVec.push_back(v0.z);
		positionVec.push_back(v3.x);
		positionVec.push_back(v3.y);
		positionVec.push_back(v3.z);
		positionVec.push_back(v7.x);
		positionVec.push_back(v7.y);
		positionVec.push_back(v7.z);

		colorVec.push_back(-n0.x);
		colorVec.push_back(-n0.y);
		colorVec.push_back(-n0.z);
		colorVec.push_back(1);
		colorVec.push_back(-n0.x);
		colorVec.push_back(-n0.y);
		colorVec.push_back(-n0.z);
		colorVec.push_back(1);
		colorVec.push_back(-n1.x);
		colorVec.push_back(-n1.y);
		colorVec.push_back(-n1.z);
		colorVec.push_back(1);

		positionVec.push_back(v0.x);
		positionVec.push_back(v0.y);
		positionVec.push_back(v0.z);
		positionVec.push_back(v7.x);
		positionVec.push_back(v7.y);
		positionVec.push_back(v7.z);
		positionVec.push_back(v4.x);
		positionVec.push_back(v4.y);
		positionVec.push_back(v4.z);

		colorVec.push_back(-n0.x);
		colorVec.push_back(-n0.y);
		colorVec.push_back(-n0.z);
		colorVec.push_back(1);
		colorVec.push_back(-n1.x);
		colorVec.push_back(-n1.y);
		colorVec.push_back(-n1.z);
		colorVec.push_back(1);
		colorVec.push_back(-n1.x);
		colorVec.push_back(-n1.y);
		colorVec.push_back(-n1.z);
		colorVec.push_back(1);

		v0 -= b0;
		v1 -= b0;
		v2 -= b0;
		v3 -= b0;
		v4 -= b1;
		v5 -= b1;
		v6 -= b1;
		v7 -= b1;

		//right face
		positionVec.push_back(v1.x);
		positionVec.push_back(v1.y);
		positionVec.push_back(v1.z);
		positionVec.push_back(v0.x);
		positionVec.push_back(v0.y);
		positionVec.push_back(v0.z);
		positionVec.push_back(v4.x);
		positionVec.push_back(v4.y);
		positionVec.push_back(v4.z);

		colorVec.push_back(b0.x);
		colorVec.push_back(b0.y);
		colorVec.push_back(b0.z);
		colorVec.push_back(1);
		colorVec.push_back(b0.x);
		colorVec.push_back(b0.y);
		colorVec.push_back(b0.z);
		colorVec.push_back(1);
		colorVec.push_back(b1.x);
		colorVec.push_back(b1.y);
		colorVec.push_back(b1.z);
		colorVec.push_back(1);


		positionVec.push_back(v1.x);
		positionVec.push_back(v1.y);
		positionVec.push_back(v1.z);
		positionVec.push_back(v4.x);
		positionVec.push_back(v4.y);
		positionVec.push_back(v4.z);
		positionVec.push_back(v5.x);
		positionVec.push_back(v5.y);
		positionVec.push_back(v5.z);

		colorVec.push_back(b0.x);
		colorVec.push_back(b0.y);
		colorVec.push_back(b0.z);
		colorVec.push_back(1);
		colorVec.push_back(b1.x);
		colorVec.push_back(b1.y);
		colorVec.push_back(b1.z);
		colorVec.push_back(1);
		colorVec.push_back(b1.x);
		colorVec.push_back(b1.y);
		colorVec.push_back(b1.z);
		colorVec.push_back(1);


		//top face
		positionVec.push_back(v2.x);
		positionVec.push_back(v2.y);
		positionVec.push_back(v2.z);
		positionVec.push_back(v1.x);
		positionVec.push_back(v1.y);
		positionVec.push_back(v1.z);
		positionVec.push_back(v5.x);
		positionVec.push_back(v5.y);
		positionVec.push_back(v5.z);

		colorVec.push_back(n0.x);
		colorVec.push_back(n0.y);
		colorVec.push_back(n0.z);
		colorVec.push_back(1);
		colorVec.push_back(n0.x);
		colorVec.push_back(n0.y);
		colorVec.push_back(n0.z);
		colorVec.push_back(1);
		colorVec.push_back(n1.x);
		colorVec.push_back(n1.y);
		colorVec.push_back(n1.z);
		colorVec.push_back(1);

		positionVec.push_back(v2.x);
		positionVec.push_back(v2.y);
		positionVec.push_back(v2.z);
		positionVec.push_back(v5.x);
		positionVec.push_back(v5.y);
		positionVec.push_back(v5.z);
		positionVec.push_back(v6.x);
		positionVec.push_back(v6.y);
		positionVec.push_back(v6.z);

		colorVec.push_back(n0.x);
		colorVec.push_back(n0.y);
		colorVec.push_back(n0.z);
		colorVec.push_back(1);
		colorVec.push_back(n1.x);
		colorVec.push_back(n1.y);
		colorVec.push_back(n1.z);
		colorVec.push_back(1);
		colorVec.push_back(n1.x);
		colorVec.push_back(n1.y);
		colorVec.push_back(n1.z);
		colorVec.push_back(1);

		//left face
		positionVec.push_back(v3.x);
		positionVec.push_back(v3.y);
		positionVec.push_back(v3.z);
		positionVec.push_back(v2.x);
		positionVec.push_back(v2.y);
		positionVec.push_back(v2.z);
		positionVec.push_back(v6.x);
		positionVec.push_back(v6.y);
		positionVec.push_back(v6.z);

		colorVec.push_back(-b0.x);
		colorVec.push_back(-b0.y);
		colorVec.push_back(-b0.z);
		colorVec.push_back(1);
		colorVec.push_back(-b0.x);
		colorVec.push_back(-b0.y);
		colorVec.push_back(-b0.z);
		colorVec.push_back(1);
		colorVec.push_back(-b1.x);
		colorVec.push_back(-b1.y);
		colorVec.push_back(-b1.z);
		colorVec.push_back(1);

		positionVec.push_back(v3.x);
		positionVec.push_back(v3.y);
		positionVec.push_back(v3.z);
		positionVec.push_back(v6.x);
		positionVec.push_back(v6.y);
		positionVec.push_back(v6.z);
		positionVec.push_back(v7.x);
		positionVec.push_back(v7.y);
		positionVec.push_back(v7.z);

		colorVec.push_back(-b0.x);
		colorVec.push_back(-b0.y);
		colorVec.push_back(-b0.z);
		colorVec.push_back(1);
		colorVec.push_back(-b1.x);
		colorVec.push_back(-b1.y);
		colorVec.push_back(-b1.z);
		colorVec.push_back(1);
		colorVec.push_back(-b1.x);
		colorVec.push_back(-b1.y);
		colorVec.push_back(-b1.z);
		colorVec.push_back(1);
		//bottom face
		positionVec.push_back(v0.x);
		positionVec.push_back(v0.y);
		positionVec.push_back(v0.z);
		positionVec.push_back(v3.x);
		positionVec.push_back(v3.y);
		positionVec.push_back(v3.z);
		positionVec.push_back(v7.x);
		positionVec.push_back(v7.y);
		positionVec.push_back(v7.z);

		colorVec.push_back(-n0.x);
		colorVec.push_back(-n0.y);
		colorVec.push_back(-n0.z);
		colorVec.push_back(1);
		colorVec.push_back(-n0.x);
		colorVec.push_back(-n0.y);
		colorVec.push_back(-n0.z);
		colorVec.push_back(1);
		colorVec.push_back(-n1.x);
		colorVec.push_back(-n1.y);
		colorVec.push_back(-n1.z);
		colorVec.push_back(1);

		positionVec.push_back(v0.x);
		positionVec.push_back(v0.y);
		positionVec.push_back(v0.z);
		positionVec.push_back(v7.x);
		positionVec.push_back(v7.y);
		positionVec.push_back(v7.z);
		positionVec.push_back(v4.x);
		positionVec.push_back(v4.y);
		positionVec.push_back(v4.z);

		colorVec.push_back(-n0.x);
		colorVec.push_back(-n0.y);
		colorVec.push_back(-n0.z);
		colorVec.push_back(1);
		colorVec.push_back(-n1.x);
		colorVec.push_back(-n1.y);
		colorVec.push_back(-n1.z);
		colorVec.push_back(1);
		colorVec.push_back(-n1.x);
		colorVec.push_back(-n1.y);
		colorVec.push_back(-n1.z);
		colorVec.push_back(1);

		u -= 0.001;
	}
	

	for (i = 1; i < splines[0].numControlPoints-3; i++) {
		for (u = 0; u < 1; u += 0.001) {
			control = glm::mat4x3((const float)splines[0].points[i - 1].x, (const float)splines[0].points[i - 1].y, (const float)splines[0].points[i - 1].z,
				(const float)splines[0].points[i].x, (const float)splines[0].points[i].y, (const float)splines[0].points[i].z,
				(const float)splines[0].points[i + 1].x, (const float)splines[0].points[i + 1].y, (const float)splines[0].points[i + 1].z,
				(const float)splines[0].points[i + 2].x, (const float)splines[0].points[i + 2].y, (const float)splines[0].points[i + 2].z);
			combined = control * basis;
			position = combined * glm::vec4(u*u*u, u*u, u, 1);
			tangent = combined * glm::vec4(3 * u * u, 2 * u, 1, 0);
			glm::vec3 n0 = glm::normalize(glm::cross(tangent, v));
			glm::vec3 b0 = glm::normalize(glm::cross(tangent, n0));
			glm::vec3 v0 = position + alpha * (-n0 + b0);
			glm::vec3 v1 = position + alpha * (n0 + b0);
			glm::vec3 v2 = position + alpha * (n0 - b0);
			glm::vec3 v3 = position + alpha * (-n0 - b0);
			normalVec.push_back(n0);
			splinePositionVec.push_back(position);
			tangentVec.push_back(tangent);

			u += 0.001;
			control = glm::mat4x3((const float)splines[0].points[i - 1].x, (const float)splines[0].points[i - 1].y, (const float)splines[0].points[i - 1].z,
				(const float)splines[0].points[i].x, (const float)splines[0].points[i].y, (const float)splines[0].points[i].z,
				(const float)splines[0].points[i + 1].x, (const float)splines[0].points[i + 1].y, (const float)splines[0].points[i + 1].z,
				(const float)splines[0].points[i + 2].x, (const float)splines[0].points[i + 2].y, (const float)splines[0].points[i + 2].z);
			combined = control * basis;
			position = combined * glm::vec4(u*u*u, u*u, u, 1);
			tangent = combined * glm::vec4(3 * u * u, 2 * u, 1, 0);
			glm::vec3 n1 = glm::normalize(glm::cross(tangent, v));
			glm::vec3 b1 = glm::normalize(glm::cross(tangent, n1));
			glm::vec3 v4 = position + alpha * (-n1 + b1);
			glm::vec3 v5 = position + alpha * (n1 + b1);
			glm::vec3 v6 = position + alpha * (n1 - b1);
			glm::vec3 v7 = position + alpha * (-n1 - b1);

			v0 += 0.5f * b0;
			v1 += 0.5f * b0;
			v2 += 0.5f * b0;
			v3 += 0.5f * b0;
			v4 += 0.5f * b1;
			v5 += 0.5f * b1;
			v6 += 0.5f * b1;
			v7 += 0.5f * b1;

			//right face
			positionVec.push_back(v1.x);
			positionVec.push_back(v1.y);
			positionVec.push_back(v1.z);
			positionVec.push_back(v0.x);
			positionVec.push_back(v0.y);
			positionVec.push_back(v0.z);
			positionVec.push_back(v4.x);
			positionVec.push_back(v4.y);
			positionVec.push_back(v4.z);

			colorVec.push_back(b0.x);
			colorVec.push_back(b0.y);
			colorVec.push_back(b0.z);
			colorVec.push_back(1);
			colorVec.push_back(b0.x);
			colorVec.push_back(b0.y);
			colorVec.push_back(b0.z);
			colorVec.push_back(1);
			colorVec.push_back(b1.x);
			colorVec.push_back(b1.y);
			colorVec.push_back(b1.z);
			colorVec.push_back(1);


			positionVec.push_back(v1.x);
			positionVec.push_back(v1.y);
			positionVec.push_back(v1.z);
			positionVec.push_back(v4.x);
			positionVec.push_back(v4.y);
			positionVec.push_back(v4.z);
			positionVec.push_back(v5.x);
			positionVec.push_back(v5.y);
			positionVec.push_back(v5.z);

			colorVec.push_back(b0.x);
			colorVec.push_back(b0.y);
			colorVec.push_back(b0.z);
			colorVec.push_back(1);
			colorVec.push_back(b1.x);
			colorVec.push_back(b1.y);
			colorVec.push_back(b1.z);
			colorVec.push_back(1);
			colorVec.push_back(b1.x);
			colorVec.push_back(b1.y);
			colorVec.push_back(b1.z);
			colorVec.push_back(1);


			//top face
			positionVec.push_back(v2.x);
			positionVec.push_back(v2.y);
			positionVec.push_back(v2.z);
			positionVec.push_back(v1.x);
			positionVec.push_back(v1.y);
			positionVec.push_back(v1.z);
			positionVec.push_back(v5.x);
			positionVec.push_back(v5.y);
			positionVec.push_back(v5.z);

			colorVec.push_back(n0.x);
			colorVec.push_back(n0.y);
			colorVec.push_back(n0.z);
			colorVec.push_back(1);
			colorVec.push_back(n0.x);
			colorVec.push_back(n0.y);
			colorVec.push_back(n0.z);
			colorVec.push_back(1);
			colorVec.push_back(n1.x);
			colorVec.push_back(n1.y);
			colorVec.push_back(n1.z);
			colorVec.push_back(1);

			positionVec.push_back(v2.x);
			positionVec.push_back(v2.y);
			positionVec.push_back(v2.z);
			positionVec.push_back(v5.x);
			positionVec.push_back(v5.y);
			positionVec.push_back(v5.z);
			positionVec.push_back(v6.x);
			positionVec.push_back(v6.y);
			positionVec.push_back(v6.z);

			colorVec.push_back(n0.x);
			colorVec.push_back(n0.y);
			colorVec.push_back(n0.z);
			colorVec.push_back(1);
			colorVec.push_back(n1.x);
			colorVec.push_back(n1.y);
			colorVec.push_back(n1.z);
			colorVec.push_back(1);
			colorVec.push_back(n1.x);
			colorVec.push_back(n1.y);
			colorVec.push_back(n1.z);
			colorVec.push_back(1);

			//left face
			positionVec.push_back(v3.x);
			positionVec.push_back(v3.y);
			positionVec.push_back(v3.z);
			positionVec.push_back(v2.x);
			positionVec.push_back(v2.y);
			positionVec.push_back(v2.z);
			positionVec.push_back(v6.x);
			positionVec.push_back(v6.y);
			positionVec.push_back(v6.z);

			colorVec.push_back(-b0.x);
			colorVec.push_back(-b0.y);
			colorVec.push_back(-b0.z);
			colorVec.push_back(1);
			colorVec.push_back(-b0.x);
			colorVec.push_back(-b0.y);
			colorVec.push_back(-b0.z);
			colorVec.push_back(1);
			colorVec.push_back(-b1.x);
			colorVec.push_back(-b1.y);
			colorVec.push_back(-b1.z);
			colorVec.push_back(1);

			positionVec.push_back(v3.x);
			positionVec.push_back(v3.y);
			positionVec.push_back(v3.z);
			positionVec.push_back(v6.x);
			positionVec.push_back(v6.y);
			positionVec.push_back(v6.z);
			positionVec.push_back(v7.x);
			positionVec.push_back(v7.y);
			positionVec.push_back(v7.z);

			colorVec.push_back(-b0.x);
			colorVec.push_back(-b0.y);
			colorVec.push_back(-b0.z);
			colorVec.push_back(1);
			colorVec.push_back(-b1.x);
			colorVec.push_back(-b1.y);
			colorVec.push_back(-b1.z);
			colorVec.push_back(1);
			colorVec.push_back(-b1.x);
			colorVec.push_back(-b1.y);
			colorVec.push_back(-b1.z);
			colorVec.push_back(1);
			//bottom face
			positionVec.push_back(v0.x);
			positionVec.push_back(v0.y);
			positionVec.push_back(v0.z);
			positionVec.push_back(v3.x);
			positionVec.push_back(v3.y);
			positionVec.push_back(v3.z);
			positionVec.push_back(v7.x);
			positionVec.push_back(v7.y);
			positionVec.push_back(v7.z);

			colorVec.push_back(-n0.x);
			colorVec.push_back(-n0.y);
			colorVec.push_back(-n0.z);
			colorVec.push_back(1);
			colorVec.push_back(-n0.x);
			colorVec.push_back(-n0.y);
			colorVec.push_back(-n0.z);
			colorVec.push_back(1);
			colorVec.push_back(-n1.x);
			colorVec.push_back(-n1.y);
			colorVec.push_back(-n1.z);
			colorVec.push_back(1);

			positionVec.push_back(v0.x);
			positionVec.push_back(v0.y);
			positionVec.push_back(v0.z);
			positionVec.push_back(v7.x);
			positionVec.push_back(v7.y);
			positionVec.push_back(v7.z);
			positionVec.push_back(v4.x);
			positionVec.push_back(v4.y);
			positionVec.push_back(v4.z);

			colorVec.push_back(-n0.x);
			colorVec.push_back(-n0.y);
			colorVec.push_back(-n0.z);
			colorVec.push_back(1);
			colorVec.push_back(-n1.x);
			colorVec.push_back(-n1.y);
			colorVec.push_back(-n1.z);
			colorVec.push_back(1);
			colorVec.push_back(-n1.x);
			colorVec.push_back(-n1.y);
			colorVec.push_back(-n1.z);
			colorVec.push_back(1);

			v0 -= b0;
			v1 -= b0;
			v2 -= b0;
			v3 -= b0;
			v4 -= b1;
			v5 -= b1;
			v6 -= b1;
			v7 -= b1;

			//right face
			positionVec.push_back(v1.x);
			positionVec.push_back(v1.y);
			positionVec.push_back(v1.z);
			positionVec.push_back(v0.x);
			positionVec.push_back(v0.y);
			positionVec.push_back(v0.z);
			positionVec.push_back(v4.x);
			positionVec.push_back(v4.y);
			positionVec.push_back(v4.z);

			colorVec.push_back(b0.x);
			colorVec.push_back(b0.y);
			colorVec.push_back(b0.z);
			colorVec.push_back(1);
			colorVec.push_back(b0.x);
			colorVec.push_back(b0.y);
			colorVec.push_back(b0.z);
			colorVec.push_back(1);
			colorVec.push_back(b1.x);
			colorVec.push_back(b1.y);
			colorVec.push_back(b1.z);
			colorVec.push_back(1);


			positionVec.push_back(v1.x);
			positionVec.push_back(v1.y);
			positionVec.push_back(v1.z);
			positionVec.push_back(v4.x);
			positionVec.push_back(v4.y);
			positionVec.push_back(v4.z);
			positionVec.push_back(v5.x);
			positionVec.push_back(v5.y);
			positionVec.push_back(v5.z);

			colorVec.push_back(b0.x);
			colorVec.push_back(b0.y);
			colorVec.push_back(b0.z);
			colorVec.push_back(1);
			colorVec.push_back(b1.x);
			colorVec.push_back(b1.y);
			colorVec.push_back(b1.z);
			colorVec.push_back(1);
			colorVec.push_back(b1.x);
			colorVec.push_back(b1.y);
			colorVec.push_back(b1.z);
			colorVec.push_back(1);


			//top face
			positionVec.push_back(v2.x);
			positionVec.push_back(v2.y);
			positionVec.push_back(v2.z);
			positionVec.push_back(v1.x);
			positionVec.push_back(v1.y);
			positionVec.push_back(v1.z);
			positionVec.push_back(v5.x);
			positionVec.push_back(v5.y);
			positionVec.push_back(v5.z);

			colorVec.push_back(n0.x);
			colorVec.push_back(n0.y);
			colorVec.push_back(n0.z);
			colorVec.push_back(1);
			colorVec.push_back(n0.x);
			colorVec.push_back(n0.y);
			colorVec.push_back(n0.z);
			colorVec.push_back(1);
			colorVec.push_back(n1.x);
			colorVec.push_back(n1.y);
			colorVec.push_back(n1.z);
			colorVec.push_back(1);

			positionVec.push_back(v2.x);
			positionVec.push_back(v2.y);
			positionVec.push_back(v2.z);
			positionVec.push_back(v5.x);
			positionVec.push_back(v5.y);
			positionVec.push_back(v5.z);
			positionVec.push_back(v6.x);
			positionVec.push_back(v6.y);
			positionVec.push_back(v6.z);

			colorVec.push_back(n0.x);
			colorVec.push_back(n0.y);
			colorVec.push_back(n0.z);
			colorVec.push_back(1);
			colorVec.push_back(n1.x);
			colorVec.push_back(n1.y);
			colorVec.push_back(n1.z);
			colorVec.push_back(1);
			colorVec.push_back(n1.x);
			colorVec.push_back(n1.y);
			colorVec.push_back(n1.z);
			colorVec.push_back(1);

			//left face
			positionVec.push_back(v3.x);
			positionVec.push_back(v3.y);
			positionVec.push_back(v3.z);
			positionVec.push_back(v2.x);
			positionVec.push_back(v2.y);
			positionVec.push_back(v2.z);
			positionVec.push_back(v6.x);
			positionVec.push_back(v6.y);
			positionVec.push_back(v6.z);

			colorVec.push_back(-b0.x);
			colorVec.push_back(-b0.y);
			colorVec.push_back(-b0.z);
			colorVec.push_back(1);
			colorVec.push_back(-b0.x);
			colorVec.push_back(-b0.y);
			colorVec.push_back(-b0.z);
			colorVec.push_back(1);
			colorVec.push_back(-b1.x);
			colorVec.push_back(-b1.y);
			colorVec.push_back(-b1.z);
			colorVec.push_back(1);

			positionVec.push_back(v3.x);
			positionVec.push_back(v3.y);
			positionVec.push_back(v3.z);
			positionVec.push_back(v6.x);
			positionVec.push_back(v6.y);
			positionVec.push_back(v6.z);
			positionVec.push_back(v7.x);
			positionVec.push_back(v7.y);
			positionVec.push_back(v7.z);

			colorVec.push_back(-b0.x);
			colorVec.push_back(-b0.y);
			colorVec.push_back(-b0.z);
			colorVec.push_back(1);
			colorVec.push_back(-b1.x);
			colorVec.push_back(-b1.y);
			colorVec.push_back(-b1.z);
			colorVec.push_back(1);
			colorVec.push_back(-b1.x);
			colorVec.push_back(-b1.y);
			colorVec.push_back(-b1.z);
			colorVec.push_back(1);
			//bottom face
			positionVec.push_back(v0.x);
			positionVec.push_back(v0.y);
			positionVec.push_back(v0.z);
			positionVec.push_back(v3.x);
			positionVec.push_back(v3.y);
			positionVec.push_back(v3.z);
			positionVec.push_back(v7.x);
			positionVec.push_back(v7.y);
			positionVec.push_back(v7.z);

			colorVec.push_back(-n0.x);
			colorVec.push_back(-n0.y);
			colorVec.push_back(-n0.z);
			colorVec.push_back(1);
			colorVec.push_back(-n0.x);
			colorVec.push_back(-n0.y);
			colorVec.push_back(-n0.z);
			colorVec.push_back(1);
			colorVec.push_back(-n1.x);
			colorVec.push_back(-n1.y);
			colorVec.push_back(-n1.z);
			colorVec.push_back(1);

			positionVec.push_back(v0.x);
			positionVec.push_back(v0.y);
			positionVec.push_back(v0.z);
			positionVec.push_back(v7.x);
			positionVec.push_back(v7.y);
			positionVec.push_back(v7.z);
			positionVec.push_back(v4.x);
			positionVec.push_back(v4.y);
			positionVec.push_back(v4.z);

			colorVec.push_back(-n0.x);
			colorVec.push_back(-n0.y);
			colorVec.push_back(-n0.z);
			colorVec.push_back(1);
			colorVec.push_back(-n1.x);
			colorVec.push_back(-n1.y);
			colorVec.push_back(-n1.z);
			colorVec.push_back(1);
			colorVec.push_back(-n1.x);
			colorVec.push_back(-n1.y);
			colorVec.push_back(-n1.z);
			colorVec.push_back(1);
			u -= 0.001;
		}
	}

	/*i = splines[0].numControlPoints - 2;
	for (u = 0; u < 1; u += 0.001) {
		control = glm::mat4x3((const float)splines[0].points[i - 1].x, (const float)splines[0].points[i - 1].y, (const float)splines[0].points[i - 1].z,
			(const float)splines[0].points[i].x, (const float)splines[0].points[i].y, (const float)splines[0].points[i].z,
			(const float)splines[0].points[i + 1].x, (const float)splines[0].points[i + 1].y, (const float)splines[0].points[i + 1].z,
			(const float)splines[0].points[i + 2].x, (const float)splines[0].points[i + 2].y, (const float)splines[0].points[i + 2].z);
		combined = control * basis;
		result = combined * glm::vec4(u*u*u, u*u, u, 1);
		positionVec.push_back(result.x);
		positionVec.push_back(result.y);
		positionVec.push_back(result.z);
		colorVec.push_back(1);
		colorVec.push_back(1);
		colorVec.push_back(1);
		colorVec.push_back(1);
	}*/
	
}

//clears both global vectors 
void clearAll() {
	std::vector<float>().swap(positionVec);
	std::vector<float>().swap(colorVec);
}

// write a screenshot to the specified filename
void saveScreenshot(const char * filename)
{
  unsigned char * screenshotData = new unsigned char[windowWidth * windowHeight * 3];
  glReadPixels(0, 0, windowWidth, windowHeight, GL_RGB, GL_UNSIGNED_BYTE, screenshotData);

  ImageIO screenshotImg(windowWidth, windowHeight, 3, screenshotData);

  if (screenshotImg.save(filename, ImageIO::FORMAT_JPEG) == ImageIO::OK)
    cout << "File " << filename << " saved successfully." << endl;
  else cout << "Failed to save file " << filename << '.' << endl;
   
  delete [] screenshotData;
}

void setTextureUnit(GLint unit)
{
	GLuint program = texPipelineProgram.GetProgramHandle();
	glActiveTexture(unit); // select the active texture unit
	// get a handle to the “textureImage” shader variable
	GLint h_textureImage = glGetUniformLocation(program, "textureImage");
	// deem the shader variable “textureImage” to read from texture unit “unit”
	glUniform1i(h_textureImage, unit - GL_TEXTURE0);
}

void displayTracks() {
	pipelineProgram.Bind();
	GLuint program = pipelineProgram.GetProgramHandle();
	h_modelViewMatrix = glGetUniformLocation(program, "modelViewMatrix");
	h_projectionMatrix = glGetUniformLocation(program, "projectionMatrix");
	GLint h_viewLightDirection =
		glGetUniformLocation(program, "viewLightDirection");

	openGLMatrix.SetMatrixMode(OpenGLMatrix::ModelView);
	openGLMatrix.LoadIdentity();
	float alpha = 1.0f;

	glm::vec3 eye, target;
	eye = splinePositionVec[cameraPositionIndex] + 0.2f * normalVec[cameraNormalIndex];
	target = splinePositionVec[cameraPositionIndex] + tangentVec[cameraTangentIndex];
	if (cameraNormalIndex < normalVec.size() - 1)	cameraNormalIndex++;
	if (cameraPositionIndex < splinePositionVec.size() - 1)	cameraPositionIndex++;
	if (cameraTangentIndex < tangentVec.size() - 1)	cameraTangentIndex++;


	//openGLMatrix.LookAt(0, 0, 3.1f, 0, 0, -1, 0.0f, 1.0f, 0.0f);
	openGLMatrix.LookAt(eye.x, eye.y, eye.z, target.x, target.y, target.z, normalVec[cameraNormalIndex].x, normalVec[cameraNormalIndex].y, normalVec[cameraNormalIndex].z);
	
	float view[16];
	openGLMatrix.GetMatrix(view);
	glm::vec4 lightDirection = glm::vec4(0.6f, 0.8f, 0.0f, 0.0f);
	lightDirection = glm::make_mat4(view) * lightDirection;
	float viewLightDirection[3] = { lightDirection.x, lightDirection.y, lightDirection.z };
	glUniform3fv(h_viewLightDirection, 1, viewLightDirection);
	GLint h_La = glGetUniformLocation(program, "La");
	GLint h_Ld = glGetUniformLocation(program, "Ld");
	GLint h_Ls = glGetUniformLocation(program, "Ls");
	GLint h_ka = glGetUniformLocation(program, "ka");
	GLint h_kd = glGetUniformLocation(program, "kd");
	GLint h_ks = glGetUniformLocation(program, "ks");
	GLint h_alpha = glGetUniformLocation(program, "alpha");
	glm::vec4 ambient(0.2f, 0.2f, 0.2f, 1.0f);
	glm::vec4 diffuse(0.8f, 0.8f, 0.8f, 1.0f);
	glm::vec4 specular(0.7f, 0.7f, 0.7f, 1.0f);
	glm::vec4 kAmbient(0.2f, 0.2f, 0.2f, 1.0f);
	glm::vec4 kDiffuse(0.6f, 0.6f, 0.6f, 1.0f);
	glm::vec4 kSpecular(0.7f, 0.7f, 0.7f, 1.0f);
	glUniform4fv(h_La, 1, glm::value_ptr(ambient));
	glUniform4fv(h_Ld, 1, glm::value_ptr(diffuse));
	glUniform4fv(h_Ls, 1, glm::value_ptr(specular));
	glUniform4fv(h_ka, 1, glm::value_ptr(kAmbient));
	glUniform4fv(h_kd, 1, glm::value_ptr(kDiffuse));
	glUniform4fv(h_ks, 1, glm::value_ptr(kSpecular));
	glUniform1f(h_alpha, 1.0f);

	openGLMatrix.Translate(landTranslate[0], landTranslate[1], landTranslate[2]);
	openGLMatrix.Rotate(landRotate[0], 1, 0, 0);
	openGLMatrix.Rotate(landRotate[1], 0, 1, 0);
	openGLMatrix.Rotate(landRotate[2], 0, 0, 1);
	openGLMatrix.Scale(landScale[0], landScale[1], landScale[2]);
	float m[16]; // column-major
	openGLMatrix.GetMatrix(m); // fill “m” with the matrix entries
	glUniformMatrix4fv(h_modelViewMatrix, 1, GL_FALSE, m);
	GLint h_normalMatrix =
		glGetUniformLocation(program, "normalMatrix");
	float n[16];
	openGLMatrix.GetNormalMatrix(n); // get normal matrix
	// upload n to the GPU
	glUniformMatrix4fv(h_normalMatrix, 1, GL_FALSE, n);
	openGLMatrix.SetMatrixMode(OpenGLMatrix::Projection);
	float p[16]; // column-major
	openGLMatrix.GetMatrix(p);
	const GLboolean isRowMajor = false;
	glUniformMatrix4fv(h_projectionMatrix, 1, isRowMajor, p);


	glBindVertexArray(vVao); // bind the VAO
	// bind the VBO “buffer” (must be previously created)
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	// get location index of the “position” shader variable
	GLuint loc = glGetAttribLocation(program, "position");
	glEnableVertexAttribArray(loc); // enable the “position” attribute
	const void * offset = (const void*)0;
	GLsizei stride = 0;
	GLboolean normalized = GL_FALSE;
	// set the layout of the “position” attribute data
	glVertexAttribPointer(loc, 3, GL_FLOAT, normalized, stride, offset);
	// get the location index of the “color” shader variable
	loc = glGetAttribLocation(program, "normal");
	glEnableVertexAttribArray(loc); // enable the “color” attribute
	offset = (const void*)(sizeof(float)*positionVec.size());
	stride = 0;
	normalized = GL_FALSE;
	// set the layout of the “color” attribute data
	glVertexAttribPointer(loc, 4, GL_FLOAT, normalized, stride, offset);
	glBindVertexArray(0); // unbind the VAO

	glBindVertexArray(vVao); // bind the VAO
	GLint first = 0;
	GLsizei count;
	count = 48 * ((splines[0].numControlPoints - 3) / 0.001);
	glDrawArrays(GL_TRIANGLES, first, count);
	glBindVertexArray(0); // unbind the VAO
}

void displayGround()
{
	texPipelineProgram.Bind();




	setTextureUnit(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texHandle);

	GLuint program = texPipelineProgram.GetProgramHandle();
	h_modelViewMatrix = glGetUniformLocation(program, "modelViewMatrix");
	h_projectionMatrix = glGetUniformLocation(program, "projectionMatrix");


	openGLMatrix.SetMatrixMode(OpenGLMatrix::ModelView);
	openGLMatrix.LoadIdentity();
	float alpha = 1.0f;

	glm::vec3 eye, target;
	eye = splinePositionVec[cameraPositionIndex] + 0.2f * normalVec[cameraNormalIndex];
	target = splinePositionVec[cameraPositionIndex] + tangentVec[cameraTangentIndex];
	if (cameraNormalIndex < normalVec.size() - 1)	cameraNormalIndex++;
	if (cameraPositionIndex < splinePositionVec.size() - 1)	cameraPositionIndex++;
	if (cameraTangentIndex < tangentVec.size() - 1)	cameraTangentIndex++;


	//openGLMatrix.LookAt(0, 0, 3.1f, 0, 0, -1, 0.0f, 1.0f, 0.0f);
	openGLMatrix.LookAt(eye.x, eye.y, eye.z, target.x, target.y, target.z, normalVec[cameraNormalIndex].x, normalVec[cameraNormalIndex].y, normalVec[cameraNormalIndex].z);
	openGLMatrix.Translate(landTranslate[0], landTranslate[1], landTranslate[2]);
	openGLMatrix.Rotate(landRotate[0], 1, 0, 0);
	openGLMatrix.Rotate(landRotate[1], 0, 1, 0);
	openGLMatrix.Rotate(landRotate[2], 0, 0, 1);
	openGLMatrix.Scale(landScale[0], landScale[1], landScale[2]);
	float m[16]; // column-major
	openGLMatrix.GetMatrix(m); // fill “m” with the matrix entries
	glUniformMatrix4fv(h_modelViewMatrix, 1, GL_FALSE, m);
	openGLMatrix.SetMatrixMode(OpenGLMatrix::Projection);
	float p[16]; // column-major
	openGLMatrix.GetMatrix(p);
	const GLboolean isRowMajor = false;
	glUniformMatrix4fv(h_projectionMatrix, 1, isRowMajor, p);

	glBindVertexArray(groundVAO);
	glBindBuffer(GL_ARRAY_BUFFER, groundVBO);
	GLuint loc = glGetAttribLocation(program, "position");
	glEnableVertexAttribArray(loc);
	const void * offset = (const void*)0;
	glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 0, offset);

	GLuint loc2 = glGetAttribLocation(program, "texCoord");
	glEnableVertexAttribArray(loc2);
	const void * offset2 = (const void*)(size_t)(groundPosVec.size() * sizeof(float));
	glVertexAttribPointer(loc2, 2, GL_FLOAT, GL_FALSE, 0, offset2);
	glBindVertexArray(0);

	glBindVertexArray(groundVAO);
	GLint first = 0;
	GLsizei numberOfVertices = (groundPosVec.size() / 3);
	glDrawArrays(GL_TRIANGLES, first, numberOfVertices);

	glBindVertexArray(0);
}

void displayFunc()
{
  // render some stuff...
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	
	//display points, lines, or triangles
	if (displayMode == PTS) {
		displayGround();

		displayTracks();
		
	}

	glutSwapBuffers();
}

void idleFunc()
{
  // do some stuff... 

  // for example, here, you can save the screenshots to disk (to make the animation)
	/*if (sscount < 999) {
		stringstream ss;
		std::string name;
		ss << sscount;
		ss >> name;
		if (sscount < 10) {
			name = "00" + name;
		}
		else if (sscount < 100) {
			name = "0" + name;
		}
		name += ".jpg";
		saveScreenshot(name.c_str());
		sscount++;
	}*/
  // make the screen update 
  glutPostRedisplay();
}

void reshapeFunc(int w, int h)
{
  glViewport(0, 0, w, h);

  // setup perspective matrix...
  openGLMatrix.SetMatrixMode(OpenGLMatrix::Projection);
  openGLMatrix.LoadIdentity();
  openGLMatrix.Perspective(45.0, 1.0 * w / h, 0.01, 1000.0);
  openGLMatrix.SetMatrixMode(OpenGLMatrix::ModelView);
}

void mouseMotionDragFunc(int x, int y)
{
  // mouse has moved and one of the mouse buttons is pressed (dragging)

  // the change in mouse position since the last invocation of this function
  int mousePosDelta[2] = { x - mousePos[0], y - mousePos[1] };

  switch (controlState)
  {
    // translate the landscape
    case TRANSLATE:
      if (leftMouseButton)
      {
        // control x,y translation via the left mouse button
        landTranslate[0] += mousePosDelta[0] * 0.01f;
        landTranslate[1] -= mousePosDelta[1] * 0.01f;
      }
      if (middleMouseButton)
      {
        // control z translation via the middle mouse button
        landTranslate[2] += mousePosDelta[1] * 0.01f;
      }
      break;

    // rotate the landscape
    case ROTATE:
      if (leftMouseButton)
      {
        // control x,y rotation via the left mouse button
        landRotate[0] += mousePosDelta[1];
        landRotate[1] += mousePosDelta[0];
      }
      if (middleMouseButton)
      {
        // control z rotation via the middle mouse button
        landRotate[2] += mousePosDelta[1];
      }
      break;

    // scale the landscape
    case SCALE:
      if (leftMouseButton)
      {
        // control x,y scaling via the left mouse button
        landScale[0] *= 1.0f + mousePosDelta[0] * 0.01f;
        landScale[1] *= 1.0f - mousePosDelta[1] * 0.01f;
      }
      if (middleMouseButton)
      {
        // control z scaling via the middle mouse button
        landScale[2] *= 1.0f - mousePosDelta[1] * 0.01f;
      }
      break;
  }

  // store the new mouse position
  mousePos[0] = x;
  mousePos[1] = y;
}

void mouseMotionFunc(int x, int y)
{
  // mouse has moved
  // store the new mouse position
  mousePos[0] = x;
  mousePos[1] = y;
}

void mouseButtonFunc(int button, int state, int x, int y)
{
  // a mouse button has has been pressed or depressed

  // keep track of the mouse button state, in leftMouseButton, middleMouseButton, rightMouseButton variables
  switch (button)
  {
    case GLUT_LEFT_BUTTON:
      leftMouseButton = (state == GLUT_DOWN);
    break;

    case GLUT_MIDDLE_BUTTON:
      middleMouseButton = (state == GLUT_DOWN);
    break;

    case GLUT_RIGHT_BUTTON:
      rightMouseButton = (state == GLUT_DOWN);
    break;
  }

  // keep track of whether CTRL and SHIFT keys are pressed
  switch (glutGetModifiers())
  {
    case GLUT_ACTIVE_CTRL:
      controlState = TRANSLATE;
    break;

    case GLUT_ACTIVE_SHIFT:
      controlState = SCALE;
    break;

    // if CTRL and SHIFT are not pressed, we are in rotate mode
    default:
      controlState = ROTATE;
    break;
  }

  // store the new mouse position
  mousePos[0] = x;
  mousePos[1] = y;
}

void keyboardFunc(unsigned char key, int x, int y)
{
  switch (key)
  {
    case 27: // ESC key
      exit(0); // exit the program
    break;

    case ' ':
      cout << "You pressed the spacebar." << endl;
    break;

    case 'x':
      // take a screenshot
      saveScreenshot("screenshot.jpg");
    break;

	case '1':
		displayMode = PTS;
		break;

	case '2':
		displayMode = LINES;
		break;

	case '3':
		displayMode = TRIANGLES;
		break;
  }
}

void initVBO() {
	float* positions = &positionVec[0];
	float* colors = &colorVec[0];
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*positionVec.size() + sizeof(float)*colorVec.size(),
		nullptr, GL_STATIC_DRAW); // init VBO’s size, but don’t assign any data to it
		// upload position data
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float)*positionVec.size(), positions);
	// upload color data
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(float)*positionVec.size(), sizeof(float)*colorVec.size(), colors);

	positions = &groundPosVec[0];
	float* UVs = &groundUVVec[0];
	glGenBuffers(1, &groundVBO);
	glBindBuffer(GL_ARRAY_BUFFER, groundVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*groundPosVec.size() + sizeof(float)*groundUVVec.size(),
		nullptr, GL_STATIC_DRAW); // init VBO’s size, but don’t assign any data to it
		// upload position data
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float)*groundPosVec.size(), positions);
	// upload color data
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(float)*groundPosVec.size(), sizeof(float)*groundUVVec.size(), UVs);
}

void initScene(int argc, char *argv[])
{
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

  // do additional initialization here...
  glEnable(GL_DEPTH_TEST);

  glGenTextures(1, &texHandle);
  int code = initTexture("ground.jpg", texHandle);
  if (code != 0)
  {
	  printf("Error loading the texture image.\n");
	  exit(EXIT_FAILURE);
  }
  loadTrackBufferData(); //load buffer for triangles
  loadGroundBufferData();
  //convert buffer into arrays
  initVBO();

  pipelineProgram.Init("../openGLHelper-starterCode");
  texPipelineProgram.Init("../openGLHelper-starterCode");

  glGenVertexArrays(1, &vVao);
  glGenVertexArrays(1, &groundVAO);

}

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		printf("usage: %s <trackfile>\n", argv[0]);
		exit(0);
	}

	// load the splines from the provided filename
	loadSplines(argv[1]);

  cout << "Initializing GLUT..." << endl;
  glutInit(&argc,argv);

  cout << "Initializing OpenGL..." << endl;

  #ifdef __APPLE__
    glutInitDisplayMode(GLUT_3_2_CORE_PROFILE | GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STENCIL);
  #else
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STENCIL);
  #endif

  glutInitWindowSize(windowWidth, windowHeight);
  glutInitWindowPosition(0, 0);  
  glutCreateWindow(windowTitle);

  cout << "OpenGL Version: " << glGetString(GL_VERSION) << endl;
  cout << "OpenGL Renderer: " << glGetString(GL_RENDERER) << endl;
  cout << "Shading Language Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;

  // tells glut to use a particular display function to redraw 
  glutDisplayFunc(displayFunc);
  // perform animation inside idleFunc
  glutIdleFunc(idleFunc);
  // callback for mouse drags
  glutMotionFunc(mouseMotionDragFunc);
  // callback for idle mouse movement
  glutPassiveMotionFunc(mouseMotionFunc);
  // callback for mouse button changes
  glutMouseFunc(mouseButtonFunc);
  // callback for resizing the window
  glutReshapeFunc(reshapeFunc);
  // callback for pressing the keys on the keyboard
  glutKeyboardFunc(keyboardFunc);

  // init glew
  #ifdef __APPLE__
    // nothing is needed on Apple
  #else
    // Windows, Linux
    GLint result = glewInit();
    if (result != GLEW_OK)
    {
      cout << "error: " << glewGetErrorString(result) << endl;
      exit(EXIT_FAILURE);
    }
  #endif

  // do initialization
  initScene(argc, argv);

  // sink forever into the glut loop
  glutMainLoop();
}


