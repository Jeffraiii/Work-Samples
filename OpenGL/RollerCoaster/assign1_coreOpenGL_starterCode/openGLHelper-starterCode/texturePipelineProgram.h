#include "pipelineProgram.h"
#include <iostream>
#include <cstring>


class TexturePipelineProgram : public PipelineProgram
{
public:
	int Init(const char * shaderBasePath) {
		if (BuildShadersFromFiles(shaderBasePath, "texture.vertexShader.glsl", "texture.fragmentShader.glsl") != 0)
		{
			std::cout << "Failed to build the pipeline program." << std::endl;
			return 1;
		}

		std::cout << "Successfully built the pipeline program." << std::endl;
		return 0;
	} // init the program, "shaderBasePath" is the path to the folder containing the shaders
  void SetModelViewMatrix(const float * m); // m is column-major
  void SetProjectionMatrix(const float * m); // m is column-major

protected:
	virtual int SetShaderVariableHandles() { return 0; }

  GLint h_projectionMatrix; // handle to the projectionMatrix variable in the shader
  GLint h_modelViewMatrix; // handle to the modelViewMatrix variable in the shader
  // note: we can use the general syntax: h_name is a handle to the shader variable "name"
};

