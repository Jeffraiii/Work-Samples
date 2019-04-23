#include "stdafx.h"
#include "texture.h"
#include "game.h"
#include "Graphics.h"
#include "DirectXTK\Inc\DDSTextureLoader.h"
#include "DirectXTK\Inc\WICTextureLoader.h"
#include <string>

Texture::Texture(Graphics *graphics)
    : mGraphics(graphics)
    , mResource(nullptr)
    , mView(nullptr)
    , mWidth(0)
    , mHeight(0)
{
}

Texture::~Texture()
{
    Free();
}

void Texture::Free()
{
    if (nullptr != mView)
    {
        mView->Release();
        mView = nullptr;
    }
    if (nullptr != mResource)
    {
        mResource->Release();
        mResource = nullptr;
    }
    mWidth = 0;
    mHeight = 0;
}

bool Texture::Load(const WCHAR* fileName)
{
    Free();     // in case there was already a texture loaded here, release it

    ID3D11Device *pDev = mGraphics->GetDevice();

    std::wstring fileStr(fileName);
    std::wstring extension = fileStr.substr(fileStr.find_last_of('.'));
    HRESULT hr = -1;
    if (extension == L".dds" || extension == L".DDS")
        hr = DirectX::CreateDDSTextureFromFile(pDev, fileName, &mResource, &mView);
    else
        hr = DirectX::CreateWICTextureFromFile(pDev, fileName, &mResource, &mView);
    DbgAssert(hr == S_OK, "Problem Creating Texture From File");
    if (S_OK != hr)
        return false;
    CD3D11_TEXTURE2D_DESC textureDesc;
	/*ID3D11Texture2D* temp;
	pDev->CreateTexture2D(&textureDesc, nullptr, &temp);
	mResource = temp;*/
    ((ID3D11Texture2D*)mResource)->GetDesc(&textureDesc);
    mWidth = textureDesc.Width;
    mHeight = textureDesc.Height;
    return true;
}

#if 1	// TODO Lab 04c
Texture* Texture::StaticLoad(const WCHAR* fileName, Game* pGame)
{
    Texture* pTex = new Texture(pGame->GetGraphics());
    if (false == pTex->Load(fileName))
    {
        delete pTex;
        return nullptr;
    }
    return pTex;
}
#endif

#if 1	// TODO Lab 03k
void Texture::SetActive(int slot) const
{
	mGraphics->SetActiveTexture(slot, mView);
}
ID3D11RenderTargetView * Texture::CreateRenderTarget(int inWidth, int inHeight, DXGI_FORMAT format)
{
	// memroy leak?
	ID3D11Texture2D *pDepthTexture;
	D3D11_TEXTURE2D_DESC descDepth;
	ID3D11RenderTargetView* target;

	ZeroMemory(&descDepth, sizeof(descDepth));
	descDepth.Width = inWidth;
	descDepth.Height = inHeight;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = format;
	descDepth.SampleDesc.Count = 1;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	HRESULT hr = mGraphics->GetDevice()->CreateTexture2D(&descDepth, nullptr, reinterpret_cast<ID3D11Texture2D**>(&mResource));

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceDesc;
	ZeroMemory(&shaderResourceDesc, sizeof(shaderResourceDesc));
	shaderResourceDesc.Format = format;
	shaderResourceDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceDesc.Texture2D.MipLevels = 1;
	shaderResourceDesc.Texture2D.MostDetailedMip = 0;

	mGraphics->GetDevice()->CreateShaderResourceView(mResource, &shaderResourceDesc, &mView);
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	ZeroMemory(&shaderResourceDesc, sizeof(renderTargetViewDesc));
	renderTargetViewDesc.Format = format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;
	mGraphics->GetDevice()->CreateRenderTargetView(mResource, &renderTargetViewDesc, &target);
	return target;
}
#endif
