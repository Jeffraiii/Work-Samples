#pragma once
#include "RenderObj.h"


#define MAX_SKELETON_BONES 80

class SkinnedObj : public RenderObj
{
public:
    struct SkinConstants
    {
        Matrix4 c_skinMatrix[MAX_SKELETON_BONES];
    };
    
    SkinnedObj(Game* pGame, const Mesh* pMesh);
    ~SkinnedObj() override;
    void Draw() override;

    SkinConstants mSkinConstants;

private:
    ID3D11Buffer* mSkinBuffer;
};
