#pragma once
#include "engineMath.h"
#include "Graphics.h"

class Component;
class Game;
class Mesh;
class Shader;
class Texture;
class VertexBuffer;

class RenderObj
{
public:
    struct PerObjectConstants
    {
        Matrix4 c_modelToWorld;
    };

    RenderObj(Game* pGame, const Mesh* pMesh);
    virtual ~RenderObj();
    virtual void Update(float deltaTime);
    virtual void Draw();

    void AddComponent(Component *pComp) { mComponents.push_back(pComp); }
    Game* GetGame() { return mGame; }

    PerObjectConstants mObjectData;

protected:
    Game* mGame;
    std::vector<Component*> mComponents;
    const Mesh* mMesh;
    ID3D11Buffer* mObjectBuffer;
};
