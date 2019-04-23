#include "texturePipelineProgram.h"
#include "openGLHeader.h"

//#include <iostream>
//#include <cstring>

using namespace std;

//int TexturePipelineProgram::Init(const char * shaderBasePath) 
//{
//  if (BuildShadersFromFiles(shaderBasePath, "texture.vertexShader.glsl", "texture.fragmentShader.glsl") != 0)
//  {
//    cout << "Failed to build the pipeline program." << endl;
//    return 1;
//  }
//
//  cout << "Successfully built the pipeline program." << endl;
//  return 0;
//}

void TexturePipelineProgram::SetModelViewMatrix(const float * m) 
{
  // pass "m" to the pipeline program, as the modelview matrix
  // students need to implement this
}

void TexturePipelineProgram::SetProjectionMatrix(const float * m) 
{
  // pass "m" to the pipeline program, as the projection matrix
  // students need to implement this
}

//int TexturePipelineProgram::SetShaderVariableHandles()
//{
//  // set h_modelViewMatrix and h_projectionMatrix
//  // students need to implement this
//  return 0;
//}

