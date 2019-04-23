#pragma once
#include "Graphics.h"
#include "assetCache.h"
#include "engineMath.h"
#include "../JobManager.h"

class Animation;
class Camera;
class Mesh;
class RenderObj;
class Shader;
class Skeleton;
class Texture;
class Physics;
class VertexBuffer;

#define MAX_POINT_LIGHTS 8

class Game
{
public:
    struct PointLightData
    {
        Vector3 diffuseColor;
        float pad0;
        Vector3 specularColor;
        float pad1;
        Vector3 position;
        float specularPower;
        float innerRadius;
        float outerRadius;
        bool isEnabled;
        float pad2;
    };

    struct LightingData
    {
        Vector3	c_ambient;
        float pad;
        PointLightData c_pointLight[MAX_POINT_LIGHTS];
    };

    Game();
    ~Game();

    void Init(HWND hWnd, float width, float height);
    void Shutdown();
	void Update(float deltaTime);
    void RenderFrame();

	void OnKeyDown(uint32_t key);
	void OnKeyUp(uint32_t key);
	bool IsKeyHeld(uint32_t key) const;

    Graphics* GetGraphics() { return &mGraphics; }

    PointLightData* AllocateLight();
    void FreeLight(PointLightData* pLight);
    void SetAmbientLight(const Vector3& color) { mLightData.c_ambient = color; }
    const Vector3 &GetAmbientLight() const { return mLightData.c_ambient; }

    Shader* GetShader(const std::wstring& shaderName);
    Texture* LoadTexture(const std::wstring& fileName);
    Mesh* LoadMesh(const std::wstring& fileName);
    Skeleton* LoadSkeleton(const std::wstring& fileName);
    Animation* LoadAnimation(const std::wstring& fileName);
	Camera* GetCamera() { return mCamera; }
	Physics* mPhysics;

private:
	std::unordered_map<uint32_t, bool> m_keyIsHeld;
    Graphics mGraphics;

    Camera* mCamera;
    LightingData mLightData;
    ID3D11Buffer *mLightingBuffer;

    AssetCache<Shader> mShaderCache;
    AssetCache<Texture> mTextureCache;
    AssetCache<Mesh> mMeshCache;
    AssetCache<Skeleton> mSkeletonCache;
    AssetCache<Animation> mAnimationCache;

	JobManager mJobManager;
    
    std::vector<RenderObj*> mRenderObj;

	bool LoadLevel(const WCHAR* fileName);

	ID3D11RenderTargetView* mOffscreenTarget = nullptr;
	ID3D11RenderTargetView* mOffscreenTargetHalf = nullptr;
	ID3D11RenderTargetView* mOffscreenTargetQuarter = nullptr;
	ID3D11RenderTargetView* mOffscreenTargetQuarter1 = nullptr;
	VertexBuffer *mCopyVertexBuffer;
	ID3D11BlendState* opaque;
	ID3D11BlendState* add;

};

struct VertexPosColor
{
    Vector3 pos;
    Graphics::Color4 color;
};

struct VertexPosColorUV
{
    Vector3 pos;
    Graphics::Color4 color;
    Vector2 uv;
};

struct VertexPosNormColorUV
{
    Vector3 pos;
    Vector3 norm;
    Graphics::Color4 color;
    Vector2 uv;
};

struct VertexPosNormUV
{
    Vector3 pos;
    Vector3 norm;
    Vector2 uv;
};

struct VertexPosNormBoneUV
{
    Vector3 pos;
    Vector3 norm;
    uint8_t boneIndex[4];
    uint8_t boneWeight[4];
    Vector2 uv;
};

struct VertexPosNormTangentUV
{
	Vector3 pos;
	Vector3 norm;
	Vector3 tangent;
	Vector2 uv;
};


struct VertexPosUV
{
	Vector3 pos;
	Vector2 uv;
};