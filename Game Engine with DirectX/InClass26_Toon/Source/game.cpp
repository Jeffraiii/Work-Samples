#include "stdafx.h"
#include "Game.h"
#include "Animation.h"
#include "Camera.h"
#include "engineMath.h"
#include "Graphics.h"
#include "jsonUtil.h"
#include "mesh.h"
#include "Profiler.h"
#include "RenderCube.h"
#include "RenderObj.h"
#include "Shader.h"
#include "Skeleton.h"
#include "SkinnedObj.h"
#include "stringUtil.h"
#include "texture.h"
#include "VertexBuffer.h"
#include "Components\Character.h"
#include "Components\CollisionComponent.h"
#include "Components\followCam.h"
#include "Components\player.h"
#include "Components\DirLight.h"
#include "rapidjson\include\rapidjson\rapidjson.h"
#include "rapidjson\include\rapidjson\document.h"
#include <fstream>
#include <sstream>


Game::Game()
    : mShaderCache(this)
    , mTextureCache(this)
    , mMeshCache(this)
    , mSkeletonCache(this)
    , mAnimationCache(this)
{
}

Game::~Game()
{
}

void Game::Init(HWND hWnd, float width, float height)
{
    mJobs.Begin();

    mGraphics.InitD3D(hWnd, width, height);
    {
        D3D11_INPUT_ELEMENT_DESC inputElem[] =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexPosNormUV, pos), D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexPosNormUV, norm), D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, offsetof(VertexPosNormUV, uv), D3D11_INPUT_PER_VERTEX_DATA, 0 }
        };
        Shader* pPhong = new Shader(&mGraphics);
        pPhong->Load(L"Shaders/Phong.hlsl", inputElem, sizeof(inputElem) / sizeof(inputElem[0]));
        mShaderCache.Cache(L"Phong", pPhong);
        Shader* pUnlit = new Shader(&mGraphics);
        pUnlit->Load(L"Shaders/Unlit.hlsl", inputElem, sizeof(inputElem) / sizeof(inputElem[0]));
        mShaderCache.Cache(L"Unlit", pUnlit);
    }
    {
        D3D11_INPUT_ELEMENT_DESC inputElem[] =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexPosNormBoneUV, pos), D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexPosNormBoneUV, norm), D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "BONES", 0, DXGI_FORMAT_R8G8B8A8_UINT, 0, offsetof(VertexPosNormBoneUV, boneIndex), D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "WEIGHTS", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, offsetof(VertexPosNormBoneUV, boneWeight), D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, offsetof(VertexPosNormBoneUV, uv), D3D11_INPUT_PER_VERTEX_DATA, 0 }
        };
        Shader* pShader = new Shader(&mGraphics);
        pShader->Load(L"Shaders/SkinnedToon.hlsl", inputElem, sizeof(inputElem) / sizeof(inputElem[0]));
        mShaderCache.Cache(L"Skinned", pShader);
    }

    mCamera = new Camera(&mGraphics);
    mLightingBuffer = mGraphics.CreateGraphicsBuffer(&mLightData, sizeof(mLightData), D3D11_BIND_CONSTANT_BUFFER, D3D11_CPU_ACCESS_WRITE, D3D11_USAGE_DYNAMIC);

    LoadLevel(L"Assets/Levels/InClass26.itplevel");
}

void Game::Shutdown()
{
    mGraphics.CleanD3D();
    delete mCamera;
    mLightingBuffer->Release();
    for (RenderObj* pObj : mRenderObj)
        delete pObj;
    mJobs.End();
}

class TestJob : public JobManager::Job
{
public:
    void DoIt() override
    {
        printf("Hello World\n");
    }
};
static TestJob s_testJob;

void Game::Update(float deltaTime)
{
//    mJobs.AddJob(&s_testJob);
    for (RenderObj* pObj : mRenderObj)
    {
        pObj->Update(deltaTime);
    }
    {
        PROFILE_SCOPE(WaitForJobs);
        mJobs.WaitForJobs();
    }
}

void Game::RenderFrame()
{
    {
        PROFILE_SCOPE(Render_SetUp);
        Graphics::Color4 clearColor(0.0f, 0.2f, 0.4f, 1.0f);
        mGraphics.BeginFrame(clearColor);
        mCamera->SetActive();
        mGraphics.UploadBuffer(mLightingBuffer, &mLightData, sizeof(mLightData));
        mGraphics.GetDeviceContext()->PSSetConstantBuffers(Graphics::CONSTANT_BUFFER_LIGHTING, 1, &mLightingBuffer);
    }
    {
        PROFILE_SCOPE(Render_Objects);
        for (RenderObj* pObj : mRenderObj)
            pObj->Draw();
    }
    {
        PROFILE_SCOPE(Render_End);
        mGraphics.EndFrame();
    }
}

void Game::OnKeyDown(uint32_t key)
{
	m_keyIsHeld[key] = true;
}

void Game::OnKeyUp(uint32_t key)
{
	m_keyIsHeld[key] = false;
}

bool Game::IsKeyHeld(uint32_t key) const
{
	const auto find = m_keyIsHeld.find(key);
	if (find != m_keyIsHeld.end())
		return find->second;
	return false;
}

Game::DirLightData* Game::AllocateDirLight()
{
    return &mLightData.c_dirLight;
}

void Game::FreeDirLight(DirLightData* pLight)
{
    // does nothing
}

Shader* Game::GetShader(const std::wstring& shaderName)
{
    return mShaderCache.Get(shaderName);
}

Texture* Game::LoadTexture(const std::wstring& fileName)
{
    return mTextureCache.Load(fileName);
}

Mesh* Game::LoadMesh(const std::wstring& fileName)
{
    return mMeshCache.Load(fileName);
}

Skeleton* Game::LoadSkeleton(const std::wstring& fileName)
{
    return mSkeletonCache.Load(fileName);
}

Animation* Game::LoadAnimation(const std::wstring& fileName)
{
    return mAnimationCache.Load(fileName);
}

bool Game::LoadLevel(const WCHAR* fileName)
{
	std::ifstream file(fileName);
	if (!file.is_open())
	{
		return false;
	}

	std::stringstream fileStream;
	fileStream << file.rdbuf();
	std::string contents = fileStream.str();
	rapidjson::StringStream jsonStr(contents.c_str());
	rapidjson::Document doc;
	doc.ParseStream(jsonStr);

	if (!doc.IsObject())
	{
		return false;
	}

	std::string str = doc["metadata"]["type"].GetString();
	int ver = doc["metadata"]["version"].GetInt();

	// Check the metadata
	if (!doc["metadata"].IsObject() ||
		str != "itplevel" ||
		ver != 2)
	{
		return false;
	}

    const rapidjson::Value& camera = doc["camera"];
    if (camera.IsObject())
    {
        Vector3 pos;
        GetVectorFromJSON(camera, "position", pos);
        Quaternion rot;
        GetQuaternionFromJSON(camera, "rotation", rot);
        Matrix4 mat = Matrix4::CreateFromQuaternion(rot) * Matrix4::CreateTranslation(pos);
        mat.Invert();
        mCamera->SetViewMat(mat);
    }

    const rapidjson::Value& lighting = doc["lightingData"];
    if (lighting.IsObject())
    {
        Vector3 light;
        GetVectorFromJSON(lighting, "ambient", light);
        SetAmbientLight(light);
    }

    const rapidjson::Value& renderObjects = doc["renderObjects"];
    if (renderObjects.IsArray())
    {
        for (rapidjson::SizeType i = 0; i < renderObjects.Size(); ++i)
        {
            const rapidjson::Value& obj = renderObjects[i];
            Vector3 pos;
            GetVectorFromJSON(obj, "position", pos);
            Quaternion rot;
            GetQuaternionFromJSON(obj, "rotation", rot);
            float scale;
            GetFloatFromJSON(obj, "scale", scale);
            std::wstring mesh;
            GetWStringFromJSON(obj, "mesh", mesh);
            Mesh* pMesh = LoadMesh(mesh);
            RenderObj* pObj = nullptr;
            SkinnedObj* pSkin = nullptr;
            if (nullptr != pMesh && pMesh->IsSkinned())
                pObj = pSkin = new SkinnedObj(this, pMesh);
            else
                pObj = new RenderObj(this, pMesh);
            pObj->mObjectData.c_modelToWorld = 
                Matrix4::CreateScale(scale)
                * Matrix4::CreateFromQuaternion(rot)
                * Matrix4::CreateTranslation(pos);

            const rapidjson::Value& components = obj["components"];
            if (components.IsArray())
            {
                for (rapidjson::SizeType j = 0; j < components.Size(); ++j)
                {
                    Component* pComp = nullptr;
                    const rapidjson::Value& comp = components[j];
                    std::string type;
                    GetStringFromJSON(comp, "type", type);
                    if (type == "DirLight")
                    {
                        pComp = new DirLight(pObj);
                    }
                    else if (type == "Character")
                    {
                        pComp = new Character(pSkin);
                    }
                    else if (type == "Player")
                    {
                        pComp = new Player(pSkin);
                    }
                    else if (type == "Collision")
                    {
                        pComp = new CollisionComponent(pObj);
                    }
                    else if (type == "FollowCam")
                    {
                        pComp = new FollowCam(pObj);
                    }
                    if (nullptr != pComp)
                    {
                        pComp->LoadProperties(comp);
                        pObj->AddComponent(pComp);
                    }
                }
            }

            mRenderObj.push_back(pObj);
        }
    }


	return true;
}