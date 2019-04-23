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
#include "Components\PointLight.h"
#include "Components/player.h"
#include "Components/followCam.h"
#include "Components/CollisionComponent.h"
#include "Components/SimpleRotate.h"
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
    mGraphics.InitD3D(hWnd, width, height);
    {
        VertexPosColor vert[] =
        {
            { Vector3(0.0f,   0.5f, 0.0f), Graphics::Color4(1.0f, 0.0f, 0.0f, 1.0f) },
            { Vector3(0.45f, -0.5,  0.0f), Graphics::Color4(0.0f, 1.0f, 0.0f, 1.0f) },
            { Vector3(-0.45f, -0.5f, 0.0f), Graphics::Color4(0.0f, 0.0f, 1.0f, 1.0f) }
        };
        uint16_t index[] = { 0, 1, 2 };

        // TODO Lab 02f
        D3D11_INPUT_ELEMENT_DESC inputElem[] =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexPosColor, pos), D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, offsetof(VertexPosColor, color), D3D11_INPUT_PER_VERTEX_DATA, 0}
        };
        Shader *pShader = new Shader(&mGraphics);
        pShader->Load(L"Shaders/Mesh.hlsl", inputElem, sizeof(inputElem) / sizeof(inputElem[0]));
        mShaderCache.Cache(L"Mesh", pShader);

        VertexBuffer* pVertBuff = new VertexBuffer(&mGraphics,
            vert, sizeof(vert) / sizeof(vert[0]), sizeof(vert[0]),
            index, sizeof(index) / sizeof(index[0]), sizeof(index[0])
            );
        Mesh* pMesh = new Mesh(this, pVertBuff, mShaderCache.Get(L"Mesh"));
        mMeshCache.Cache(L"Triangle", pMesh);
        //RenderObj* pObj = new RenderObj(this, pMesh);
        //mRenderObj.push_back(pObj);
    }
	{
		VertexPosUV vert[] = {
			{ Vector3(-1.0f, -1.0f, 0.0f), Vector2(0.0f, 1.0f) },
			{ Vector3(1.0f, -1.0f, 0.0f), Vector2(1.0f, 1.0f) },
			{ Vector3(1.0f, 1.0f, 0.0f), Vector2(1.0f, 0.0f) },
			{ Vector3(-1.0f, 1.0f, 0.0f), Vector2(0.0f, 0.0f) }
			};
		uint16_t index[] = {
			0, 1, 2,
			0, 2, 3
		};
		mCopyVertexBuffer = new VertexBuffer(&mGraphics, vert, 4, sizeof(VertexPosUV), index, 6, sizeof(uint16_t));
	}
    {
        D3D11_INPUT_ELEMENT_DESC inputElem[] =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexPosNormColorUV, pos), D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexPosNormColorUV, norm), D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, offsetof(VertexPosNormColorUV, color), D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, offsetof(VertexPosNormColorUV, uv), D3D11_INPUT_PER_VERTEX_DATA, 0 }
        };
        Shader* pShader = new Shader(&mGraphics);
        pShader->Load(L"Shaders/BasicMesh.hlsl", inputElem, sizeof(inputElem) / sizeof(inputElem[0]));
        mShaderCache.Cache(L"BasicMesh", pShader);
    }
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
        pShader->Load(L"Shaders/Skinned.hlsl", inputElem, sizeof(inputElem) / sizeof(inputElem[0]));
        mShaderCache.Cache(L"Skinned", pShader);
    }
	{
		D3D11_INPUT_ELEMENT_DESC inputElem[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexPosNormTangentUV, pos), D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexPosNormTangentUV, norm), D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, offsetof(VertexPosNormTangentUV, tangent), D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, offsetof(VertexPosNormTangentUV, uv), D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};
		Shader* pShader = new Shader(&mGraphics);
		pShader->Load(L"Shaders/Normal.hlsl", inputElem, sizeof(inputElem) / sizeof(inputElem[0]));
		mShaderCache.Cache(L"Normal", pShader);
	}
	{
		D3D11_INPUT_ELEMENT_DESC inputElem[] =
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexPosUV, pos), D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{"UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0,  offsetof(VertexPosUV, uv), D3D11_INPUT_PER_VERTEX_DATA, 0},
		};
		Shader* pShader = new Shader(&mGraphics);
		pShader->Load(L"Shaders/Copy.hlsl", inputElem, sizeof(inputElem) / sizeof(inputElem[0]));
		mShaderCache.Cache(L"Copy", pShader);
	}
	{
		D3D11_INPUT_ELEMENT_DESC inputElem[] =
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexPosUV, pos), D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{"UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0,  offsetof(VertexPosUV, uv), D3D11_INPUT_PER_VERTEX_DATA, 0},
		};
		Shader* pShader = new Shader(&mGraphics);
		pShader->Load(L"Shaders/BloomMask.hlsl", inputElem, sizeof(inputElem) / sizeof(inputElem[0]));
		mShaderCache.Cache(L"BloomMask", pShader);
	}
	{
		D3D11_INPUT_ELEMENT_DESC inputElem[] =
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexPosUV, pos), D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{"UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0,  offsetof(VertexPosUV, uv), D3D11_INPUT_PER_VERTEX_DATA, 0},
		};
		Shader* pShader = new Shader(&mGraphics);
		pShader->Load(L"Shaders/GaussianHorizontal.hlsl", inputElem, sizeof(inputElem) / sizeof(inputElem[0]));
		mShaderCache.Cache(L"GaussianHorizontal", pShader);
	}
	{
		D3D11_INPUT_ELEMENT_DESC inputElem[] =
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexPosUV, pos), D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{"UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0,  offsetof(VertexPosUV, uv), D3D11_INPUT_PER_VERTEX_DATA, 0},
		};
		Shader* pShader = new Shader(&mGraphics);
		pShader->Load(L"Shaders/GaussianVertical.hlsl", inputElem, sizeof(inputElem) / sizeof(inputElem[0]));
		mShaderCache.Cache(L"GaussianVertical", pShader);
	}
	mPhysics = new Physics();

    mCamera = new Camera(&mGraphics);
    mLightingBuffer = mGraphics.CreateGraphicsBuffer(&mLightData, sizeof(mLightData), D3D11_BIND_CONSTANT_BUFFER, D3D11_CPU_ACCESS_WRITE, D3D11_USAGE_DYNAMIC);

    LoadLevel(L"Assets/Levels/Level10.itplevel");	

	mJobManager.Begin();

	Texture* tex = new Texture(&mGraphics);
	mTextureCache.Cache(L"Offscreen", tex);
	mOffscreenTarget = tex->CreateRenderTarget(width, height, DXGI_FORMAT_R32G32B32A32_FLOAT);

	tex = new Texture(&mGraphics);
	mTextureCache.Cache(L"OffscreenHalf", tex);
	mOffscreenTargetHalf = tex->CreateRenderTarget(width/2, height/2, DXGI_FORMAT_R32G32B32A32_FLOAT);

	tex = new Texture(&mGraphics);
	mTextureCache.Cache(L"OffscreenQuarter", tex);
	mOffscreenTargetQuarter = tex->CreateRenderTarget(width/4, height/4, DXGI_FORMAT_R32G32B32A32_FLOAT);

	tex = new Texture(&mGraphics);
	mTextureCache.Cache(L"OffscreenQuarter1", tex);
	mOffscreenTargetQuarter1 = tex->CreateRenderTarget(width / 4, height / 4, DXGI_FORMAT_R32G32B32A32_FLOAT);
}

void Game::Shutdown()
{
	mJobManager.End();
    mGraphics.CleanD3D();
    delete mCamera;
	delete mPhysics;
    mLightingBuffer->Release();
    for (RenderObj* pObj : mRenderObj)
        delete pObj;
}

static float s_angle = 0.0f;
void Game::Update(float deltaTime)
{
    for (RenderObj* pObj : mRenderObj)
    {
        pObj->Update(deltaTime);
    }
	mJobManager.WaitForJobs();
}

void Game::RenderFrame()
{
	mGraphics.SetBlendState(opaque);
    Graphics::Color4 clearColor(0.0f, 0.2f, 0.4f, 1.0f);
	mGraphics.SetRenderTarget(mOffscreenTarget, mGraphics.GetDepthBuffer());
    mGraphics.BeginFrame(clearColor);
    mCamera->SetActive();
    mGraphics.UploadBuffer(mLightingBuffer, &mLightData, sizeof(mLightData));
    mGraphics.GetDeviceContext()->PSSetConstantBuffers(Graphics::CONSTANT_BUFFER_LIGHTING, 1, &mLightingBuffer);

    for (RenderObj* pObj : mRenderObj)
        pObj->Draw();
	mGraphics.SetViewport(0, 0, mGraphics.GetScreenWidth() / 2, mGraphics.GetScreenHeight() / 2);
	mGraphics.SetRenderTarget(mOffscreenTargetHalf, nullptr);
	mShaderCache.Get(L"BloomMask")->SetActive();
	mCopyVertexBuffer->SetActive();
	mTextureCache.Get(L"Offscreen")->SetActive(Graphics::TEXTURE_SLOT_DIFFUSE);
	mCopyVertexBuffer->Draw();
	mGraphics.SetActiveTexture(Graphics::TEXTURE_SLOT_DIFFUSE, nullptr);

	mGraphics.SetViewport(0, 0, mGraphics.GetScreenWidth() / 4, mGraphics.GetScreenHeight() / 4);
	mGraphics.SetRenderTarget(mOffscreenTargetQuarter, nullptr);
	mShaderCache.Get(L"Copy")->SetActive();
	mTextureCache.Get(L"OffscreenHalf")->SetActive(Graphics::TEXTURE_SLOT_DIFFUSE);
	mCopyVertexBuffer->Draw();
	mGraphics.SetActiveTexture(Graphics::TEXTURE_SLOT_DIFFUSE, nullptr);

	mGraphics.SetRenderTarget(mOffscreenTargetQuarter1, nullptr);
	mShaderCache.Get(L"GaussianHorizontal")->SetActive();
	mTextureCache.Get(L"OffscreenQuarter")->SetActive(Graphics::TEXTURE_SLOT_DIFFUSE);
	mCopyVertexBuffer->Draw();
	mGraphics.SetActiveTexture(Graphics::TEXTURE_SLOT_DIFFUSE, nullptr);


	mGraphics.SetRenderTarget(mOffscreenTargetQuarter, nullptr);
	mShaderCache.Get(L"GaussianVertical")->SetActive();
	mTextureCache.Get(L"OffscreenQuarter1")->SetActive(Graphics::TEXTURE_SLOT_DIFFUSE);
	mCopyVertexBuffer->Draw();
	mGraphics.SetActiveTexture(Graphics::TEXTURE_SLOT_DIFFUSE, nullptr);

	mGraphics.SetViewport(0, 0, mGraphics.GetScreenWidth(), mGraphics.GetScreenHeight());
	mGraphics.SetRenderTarget(mGraphics.GetBackBuffer(), nullptr);
	mShaderCache.Get(L"Copy")->SetActive();
	mTextureCache.Get(L"OffscreenQuarter")->SetActive(Graphics::TEXTURE_SLOT_DIFFUSE);
	mCopyVertexBuffer->Draw();
	mGraphics.SetActiveTexture(Graphics::TEXTURE_SLOT_DIFFUSE, nullptr);

	mGraphics.SetBlendState(add);
	mGraphics.SetViewport(0, 0, mGraphics.GetScreenWidth(), mGraphics.GetScreenHeight());
	mGraphics.SetRenderTarget(mGraphics.GetBackBuffer(), nullptr);
	mShaderCache.Get(L"Copy")->SetActive();
	mTextureCache.Get(L"Offscreen")->SetActive(Graphics::TEXTURE_SLOT_DIFFUSE);
	mCopyVertexBuffer->Draw();
	mGraphics.SetActiveTexture(Graphics::TEXTURE_SLOT_DIFFUSE, nullptr);

    mGraphics.EndFrame();
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

Game::PointLightData* Game::AllocateLight()
{
    for (int i = 0; i < MAX_POINT_LIGHTS; ++i)
    {
        if (false == mLightData.c_pointLight[i].isEnabled)
        {
            mLightData.c_pointLight[i].isEnabled = true;
            return &mLightData.c_pointLight[i];
        }
    }
    return nullptr;
}

void Game::FreeLight(PointLightData* pLight)
{
    pLight->isEnabled = false;
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
                    if (type == "PointLight")
                    {
                        pComp = new PointLight(pObj);
                    }
                    else if (type == "Character")
                    {
                        pComp = new Character(pSkin);
                    }
					else if (type == "Player") {
						pComp = new Player(pSkin);
					}
					else if (type == "FollowCam") {
						pComp = new FollowCam(pObj);
					}
					else if (type == "Collision") {
						pComp = new CollisionComponent(pObj);
					}
					else if (type == "SimpleRotate") {
						pComp = new SimpleRotate(pObj);
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