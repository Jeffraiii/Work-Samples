#include "stdafx.h"
#include "RenderObj.h"
#include "Component.h"
#include "game.h"
#include "mesh.h"
#include "Shader.h"
#include "texture.h"
#include "VertexBuffer.h"

RenderObj::RenderObj(Game* pGame, const Mesh* pMesh)
    : mGame(pGame)
    , mMesh(pMesh)
{
    mObjectData.c_modelToWorld = Matrix4::Identity;
    mObjectData.c_modelToWorld = Matrix4::CreateRotationZ(Math::ToDegrees(45.0f));
    mObjectBuffer = mGame->GetGraphics()->CreateGraphicsBuffer(&mObjectData, sizeof(mObjectData), D3D11_BIND_CONSTANT_BUFFER, D3D11_CPU_ACCESS_WRITE, D3D11_USAGE_DYNAMIC);
}

RenderObj::~RenderObj()
{
    mObjectBuffer->Release();
}

void RenderObj::Update(float deltaTime)
{
    for (Component* pComp : mComponents)
    {
        pComp->Update(deltaTime);
    }
}

void RenderObj::Draw()
{
    mGame->GetGraphics()->UploadBuffer(mObjectBuffer, &mObjectData, sizeof(mObjectData));
    mGame->GetGraphics()->GetDeviceContext()->VSSetConstantBuffers(Graphics::CONSTANT_BUFFER_RENDEROBJ, 1, &mObjectBuffer);
    mMesh->Draw();
}
