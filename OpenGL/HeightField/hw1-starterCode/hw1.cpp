/*
  CSCI 420 Computer Graphics, USC
  Assignment 1: Height Fields
  C++ starter code

  Student username: tianmul
*/

#include "basicPipelineProgram.h"
#include "openGLMatrix.h"
#include "imageIO.h"
#include "openGLHeader.h"
#include "glutHeader.h"
#include <vector>
#include <iostream>
#include <cstring>
#include <sstream>

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
GLint h_modelViewMatrix, h_projectionMatrix;
GLuint vVao, lVao, tVao; //global VAOs for each display mode
//global vectors to store vertex data
std::vector<float> positionVec;
std::vector<float> colorVec;
int sscount = 0;

//loads data for points mode
void loadVBuffer() {
	float scale = 0.001;
	for (int i = 0; i < heightmapImage->getHeight(); i++) {
		for (int j = 0; j < heightmapImage->getWidth(); j++) {
			positionVec.push_back(i * 1.0f / heightmapImage->getHeight());
			positionVec.push_back(scale * heightmapImage->getPixel(i, j, 0));
			positionVec.push_back(-j * 1.0f / heightmapImage->getWidth());
			colorVec.push_back(1.0*heightmapImage->getPixel(i, j, 0) / 255);
			colorVec.push_back(1.0 - 1.0*heightmapImage->getPixel(i, j, 0) / 255);
			colorVec.push_back(1);
			colorVec.push_back(1);
		}
	}
}

//loads data for lines mode
void loadLBuffer() {
	float scale = 0.001;
	for (int i = 0; i < heightmapImage->getHeight(); i++) {
		for (int j = 0; j < heightmapImage->getWidth()-1; j++) {
			positionVec.push_back(i * 1.0f / heightmapImage->getHeight());
			positionVec.push_back(scale * heightmapImage->getPixel(i, j, 0));
			positionVec.push_back(-j * 1.0f / heightmapImage->getWidth());
			colorVec.push_back(1);
			colorVec.push_back(1.0*heightmapImage->getPixel(i, j, 0) / 255);
			colorVec.push_back(1.0 - 1.0*heightmapImage->getPixel(i, j, 0) / 255);
			colorVec.push_back(1);

			positionVec.push_back(i * 1.0f / heightmapImage->getHeight());
			positionVec.push_back(scale * heightmapImage->getPixel(i, j+1, 0));
			positionVec.push_back(-(j + 1) * 1.0f / heightmapImage->getWidth());
			colorVec.push_back(1);
			colorVec.push_back(1.0*heightmapImage->getPixel(i, j+1, 0) / 255);
			colorVec.push_back(1.0 - 1.0*heightmapImage->getPixel(i, j+1, 0) / 255);
			colorVec.push_back(1);
		}
	}

	for (int j = 0; j < heightmapImage->getWidth(); j++) {
		for (int i = 0; i < heightmapImage->getHeight()-1; i++) {
			positionVec.push_back(i * 1.0f / heightmapImage->getHeight());
			positionVec.push_back(scale * heightmapImage->getPixel(i, j, 0));
			positionVec.push_back(-j * 1.0f / heightmapImage->getWidth());
			colorVec.push_back(1);
			colorVec.push_back(1.0*heightmapImage->getPixel(i, j, 0) / 255);
			colorVec.push_back(1.0 - 1.0*heightmapImage->getPixel(i, j, 0) / 255);
			colorVec.push_back(1);

			positionVec.push_back((i+1) * 1.0f / heightmapImage->getHeight());
			positionVec.push_back(scale * heightmapImage->getPixel(i+1, j, 0));
			positionVec.push_back(-j * 1.0f / heightmapImage->getWidth());
			colorVec.push_back(1);
			colorVec.push_back(1.0*heightmapImage->getPixel(i+1, j, 0) / 255);
			colorVec.push_back(1.0 - 1.0*heightmapImage->getPixel(i+1, j, 0) / 255);
			colorVec.push_back(1);
		}
	}
}

//loads data for triangle mode
void loadTBuffer() {
	float scale = 0.001;
	for (int i = 0; i < heightmapImage->getHeight()-1; i++) {
		for (int j = 0; j < heightmapImage->getWidth()-1; j++) {
			positionVec.push_back(i * 1.0f / heightmapImage->getHeight());
			positionVec.push_back(scale * heightmapImage->getPixel(i, j, 0));
			positionVec.push_back(-j * 1.0f / heightmapImage->getWidth());
			colorVec.push_back(1.0*heightmapImage->getPixel(i, j, 0)/255);
			colorVec.push_back(1.0*heightmapImage->getPixel(i, j, 0)/255);
			colorVec.push_back(1.0*heightmapImage->getPixel(i, j, 0)/255);
			colorVec.push_back(1);

			positionVec.push_back((i+1) * 1.0f / heightmapImage->getHeight());
			positionVec.push_back(scale * heightmapImage->getPixel(i+1, j, 0));
			positionVec.push_back(-j * 1.0f / heightmapImage->getWidth());
			colorVec.push_back(1.0*heightmapImage->getPixel(i + 1, j, 0)/255);
			colorVec.push_back(1.0*heightmapImage->getPixel(i + 1, j, 0)/255);
			colorVec.push_back(1.0*heightmapImage->getPixel(i + 1, j, 0)/255);
			colorVec.push_back(1);

			positionVec.push_back((i+1) * 1.0f / heightmapImage->getHeight());
			positionVec.push_back(scale * heightmapImage->getPixel(i+1, j+1, 0));
			positionVec.push_back(-(j+1) * 1.0f / heightmapImage->getWidth());
			colorVec.push_back(1.0*heightmapImage->getPixel(i + 1, j + 1, 0)/255);
			colorVec.push_back(1.0*heightmapImage->getPixel(i + 1, j + 1, 0)/255);
			colorVec.push_back(1.0*heightmapImage->getPixel(i + 1, j + 1, 0)/255);
			colorVec.push_back(1);

			positionVec.push_back(i * 1.0f / heightmapImage->getHeight());
			positionVec.push_back(scale * heightmapImage->getPixel(i, j, 0));
			positionVec.push_back(-j * 1.0f / heightmapImage->getWidth());
			colorVec.push_back(1.0*heightmapImage->getPixel(i, j, 0)/255);
			colorVec.push_back(1.0*heightmapImage->getPixel(i, j, 0)/255);
			colorVec.push_back(1.0*heightmapImage->getPixel(i, j, 0)/255);
			colorVec.push_back(1);

			positionVec.push_back((i + 1) * 1.0f / heightmapImage->getHeight());
			positionVec.push_back(scale * heightmapImage->getPixel(i + 1, j + 1, 0));
			positionVec.push_back(-(j+1) * 1.0f / heightmapImage->getWidth());
			colorVec.push_back(1.0*heightmapImage->getPixel(i + 1, j + 1, 0)/255);
			colorVec.push_back(1.0*heightmapImage->getPixel(i + 1, j + 1, 0)/255);
			colorVec.push_back(1.0*heightmapImage->getPixel(i + 1, j + 1, 0)/255);
			colorVec.push_back(1);

			positionVec.push_back(i * 1.0f / heightmapImage->getHeight());
			positionVec.push_back(scale * heightmapImage->getPixel(i, j+1, 0));
			positionVec.push_back(-(j+1) * 1.0f / heightmapImage->getWidth());
			colorVec.push_back(1.0* heightmapImage->getPixel(i, j + 1, 0)/255);
			colorVec.push_back(1.0* heightmapImage->getPixel(i, j + 1, 0) / 255);
			colorVec.push_back(1.0* heightmapImage->getPixel(i, j + 1, 0) / 255);
			colorVec.push_back(1);
		}
	}
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

void displayFunc()
{
  // render some stuff...
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	openGLMatrix.SetMatrixMode(OpenGLMatrix::ModelView);
	openGLMatrix.LoadIdentity();
	float alpha = 1.0f;
	openGLMatrix.LookAt(1.0f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f);
    openGLMatrix.Translate(landTranslate[0],landTranslate[1],landTranslate[2]);
    openGLMatrix.Rotate(landRotate[0],1,0,0);
    openGLMatrix.Rotate(landRotate[1],0,1,0);
    openGLMatrix.Rotate(landRotate[2],0,0,1);
    openGLMatrix.Scale(landScale[0],landScale[1],landScale[2]);
	float m[16]; // column-major
	openGLMatrix.GetMatrix(m); // fill “m” with the matrix entries
	glUniformMatrix4fv(h_modelViewMatrix, 1, GL_FALSE, m);
    openGLMatrix.SetMatrixMode(OpenGLMatrix::Projection);
    float p[16]; // column-major
    openGLMatrix.GetMatrix(p);
    const GLboolean isRowMajor = false;
    glUniformMatrix4fv(h_projectionMatrix, 1, isRowMajor, p);
	//display points, lines, or triangles
	GLsizei count;
	if (displayMode == PTS) {
		glBindVertexArray(vVao); // bind the VAO
		GLint first = 0;
		count = heightmapImage->getHeight() * heightmapImage->getWidth();
		glDrawArrays(GL_POINTS, first, count);

	}
	else if (displayMode == LINES) {
		glBindVertexArray(lVao); // bind the VAO
		GLint first = 0;
		count = 2 * ((heightmapImage->getHeight() - 1) * heightmapImage->getWidth() + heightmapImage->getHeight() *(heightmapImage->getWidth() - 1));
		glDrawArrays(GL_LINES, first, count);

	}
	else if (displayMode == TRIANGLES) {
		glBindVertexArray(tVao); // bind the VAO
		GLint first = 0;
		count = 6 * (heightmapImage->getHeight() - 1)*(heightmapImage->getWidth() - 1);
		glDrawArrays(GL_TRIANGLES, first, count);

	}

	glBindVertexArray(0); // unbind the VAO
	glutSwapBuffers();
}

void idleFunc()
{
  // do some stuff... 

  // for example, here, you can save the screenshots to disk (to make the animation)
	if (sscount < 300) {
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
	}
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

void initScene(int argc, char *argv[])
{
  // load the image from a jpeg disk file to main memory
  heightmapImage = new ImageIO();
  if (heightmapImage->loadJPEG(argv[1]) != ImageIO::OK)
  {
    cout << "Error reading image " << argv[1] << "." << endl;
    exit(EXIT_FAILURE);
  }
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

  // do additional initialization here...
  glEnable(GL_DEPTH_TEST);

  loadTBuffer(); //load buffer for triangles
  //convert buffer into arrays
  float* positions = &positionVec[0];
  float* colors = &colorVec[0];


  pipelineProgram.Init("../openGLHelper-starterCode");
  pipelineProgram.Bind();
  GLuint program = pipelineProgram.GetProgramHandle();
  h_modelViewMatrix = glGetUniformLocation(program, "modelViewMatrix");
  h_projectionMatrix = glGetUniformLocation(program, "projectionMatrix");

  GLuint vbo;
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float)*positionVec.size() + sizeof(float)*colorVec.size(),
	  nullptr, GL_STATIC_DRAW); // init VBO’s size, but don’t assign any data to it
	  // upload position data
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float)*positionVec.size(), positions);
  // upload color data
  glBufferSubData(GL_ARRAY_BUFFER, sizeof(float)*positionVec.size(), sizeof(float)*colorVec.size(), colors);

  glGenVertexArrays(1, &tVao);
  glBindVertexArray(tVao); // bind the VAO
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
  loc = glGetAttribLocation(program, "color");
  glEnableVertexAttribArray(loc); // enable the “color” attribute
  offset = (const void*) (sizeof(float)*positionVec.size());
  stride = 0;
  normalized = GL_FALSE;
  // set the layout of the “color” attribute data
  glVertexAttribPointer(loc, 4, GL_FLOAT, normalized, stride, offset);
  glBindVertexArray(0); // unbind the VAO

  clearAll(); //clear buffer
  loadLBuffer(); //load new buffer for lines
  positions = &positionVec[0];
  colors = &colorVec[0];

  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float)*positionVec.size() + sizeof(float)*colorVec.size(),
	  nullptr, GL_STATIC_DRAW); // init VBO’s size, but don’t assign any data to it
	  // upload position data
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float)*positionVec.size(), positions);
  // upload color data
  glBufferSubData(GL_ARRAY_BUFFER, sizeof(float)*positionVec.size(), sizeof(float)*colorVec.size(), colors);

  glGenVertexArrays(1, &lVao);
  glBindVertexArray(lVao); // bind the VAO
  // bind the VBO “buffer” (must be previously created)
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  // get location index of the “position” shader variable
   loc = glGetAttribLocation(program, "position");
  glEnableVertexAttribArray(loc); // enable the “position” attribute
offset = (const void*)0;
   stride = 0;
   normalized = GL_FALSE;
  // set the layout of the “position” attribute data
  glVertexAttribPointer(loc, 3, GL_FLOAT, normalized, stride, offset);
  // get the location index of the “color” shader variable
  loc = glGetAttribLocation(program, "color");
  glEnableVertexAttribArray(loc); // enable the “color” attribute
  offset = (const void*)(sizeof(float)*positionVec.size());
  stride = 0;
  normalized = GL_FALSE;
  // set the layout of the “color” attribute data
  glVertexAttribPointer(loc, 4, GL_FLOAT, normalized, stride, offset);
  glBindVertexArray(0); // unbind the VAO

  clearAll();//clear buffer
  loadVBuffer();//load new buffer for points
  positions = &positionVec[0];
  colors = &colorVec[0];

  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float)*positionVec.size() + sizeof(float)*colorVec.size(),
	  nullptr, GL_STATIC_DRAW); // init VBO’s size, but don’t assign any data to it
	  // upload position data
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float)*positionVec.size(), positions);
  // upload color data
  glBufferSubData(GL_ARRAY_BUFFER, sizeof(float)*positionVec.size(), sizeof(float)*colorVec.size(), colors);

  glGenVertexArrays(1, &vVao);
  glBindVertexArray(vVao); // bind the VAO
  // bind the VBO “buffer” (must be previously created)
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  // get location index of the “position” shader variable
   loc = glGetAttribLocation(program, "position");
  glEnableVertexAttribArray(loc); // enable the “position” attribute
 offset = (const void*)0;
   stride = 0;
   normalized = GL_FALSE;
  // set the layout of the “position” attribute data
  glVertexAttribPointer(loc, 3, GL_FLOAT, normalized, stride, offset);
  // get the location index of the “color” shader variable
  loc = glGetAttribLocation(program, "color");
  glEnableVertexAttribArray(loc); // enable the “color” attribute
  offset = (const void*)(sizeof(float)*positionVec.size());
  stride = 0;
  normalized = GL_FALSE;
  // set the layout of the “color” attribute data
  glVertexAttribPointer(loc, 4, GL_FLOAT, normalized, stride, offset);
  glBindVertexArray(0); // unbind the VAO
}

int main(int argc, char *argv[])
{
  if (argc != 2)
  {
    cout << "The arguments are incorrect." << endl;
    cout << "usage: ./hw1 <heightmap file>" << endl;
    exit(EXIT_FAILURE);
  }

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


