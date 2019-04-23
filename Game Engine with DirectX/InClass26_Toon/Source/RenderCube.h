#pragma once
#include "RenderObj.h"

class RenderCube : public RenderObj
{
public:
    RenderCube(Game *pGame, const Shader *pShader, const Texture *pTex);
    ~RenderCube();
};