#include "stdafx.h"
#include "Graphics.h"
#include "Shader.h"
#include "engineMath.h"

#pragma comment (lib, "d3d11.lib") 

Graphics::Graphics()
    : mScreenWidth(0)
    , mScreenHeight(0)
{
}

Graphics::~Graphics()
{
}

void Graphics::InitD3D(HWND hWnd, float width, float height)
{
    mScreenWidth = width;
    mScreenHeight = height;

    // clear out the struct for use
    DXGI_SWAP_CHAIN_DESC scd;
    ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));
    // fill the swap chain description struct
    scd.BufferCount = 1;                                 // one back buffer
    scd.BufferDesc.Width = (UINT)mScreenWidth;
    scd.BufferDesc.Height = (UINT)mScreenHeight;
    scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;  // use 32-bit color
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;   // how swap chain is to be used
    scd.OutputWindow = hWnd;                             // the window to be used
    scd.SampleDesc.Count = 1;                            // how many multisamples
    scd.Windowed = TRUE;                                 // windowed/full-screen mode
    HRESULT hr = D3D11CreateDeviceAndSwapChain(NULL,
        D3D_DRIVER_TYPE_HARDWARE,
        NULL,
        D3D11_CREATE_DEVICE_DEBUG,
        NULL,
        NULL,
        D3D11_SDK_VERSION,
        &scd,
        &mSwapchain,
        &mDev,
        NULL,
        &mDevcon);
    D3D11_VIEWPORT viewport;
    ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
    viewport.Width = width;
    viewport.Height = height;
    viewport.MaxDepth = 1.0f;
    viewport.MinDepth = 0.0f;
    mDevcon->RSSetViewports(1, &viewport);

    CreateDepthStencil((int)mScreenWidth, (int)mScreenHeight, &mDepthStencilTexture, &mDepthStencilView);
    mDepthState = CreateDepthStencilState(true, D3D11_COMPARISON_LESS_EQUAL);

    ID3D11Texture2D *pBackBuffer;
    hr = mSwapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
    DbgAssert(hr == S_OK, "Something wrong with your back buffer");
    mDev->CreateRenderTargetView(pBackBuffer, nullptr, &mBackbuffer);
    pBackBuffer->Release();
    mDevcon->OMSetRenderTargets(1, &mBackbuffer, mDepthStencilView);
    mDevcon->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    mDevcon->OMSetDepthStencilState(mDepthState, 0);

    D3D11_SAMPLER_DESC sampDesc;
    ZeroMemory(&sampDesc, sizeof(sampDesc));
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
    hr = mDev->CreateSamplerState(&sampDesc, &mDefaultSampler);
    SetActiveSampler(0, mDefaultSampler);

    {
        D3D11_RASTERIZER_DESC rasterDesc;
        ZeroMemory(&rasterDesc, sizeof(rasterDesc));
        rasterDesc.FillMode = D3D11_FILL_SOLID;
        rasterDesc.CullMode = D3D11_CULL_BACK;
        rasterDesc.FrontCounterClockwise = true;
        ID3D11RasterizerState* pRasterState;
        mDev->CreateRasterizerState(&rasterDesc, &pRasterState);
        mDevcon->RSSetState(pRasterState);
        pRasterState->Release();
    }
}

void Graphics::BeginFrame(const Color4 &clearColor)
{
    mDevcon->ClearRenderTargetView(currBuffer, (float*)&clearColor);
    mDevcon->ClearDepthStencilView(currDepth, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void Graphics::EndFrame()
{
    mSwapchain->Present(1, 0);
}

void Graphics::CleanD3D()
{
    mSwapchain->Release();
    mDev->Release();
    mDevcon->Release();
    mBackbuffer->Release();
    mDepthState->Release();
    mDepthStencilView->Release();
    mDepthStencilTexture->Release();
    mDefaultSampler->Release();
}

bool Graphics::CreateDepthStencil(int inWidth, int inHeight, ID3D11Texture2D **pDepthTexture, ID3D11DepthStencilView **pDepthView)
{
    D3D11_TEXTURE2D_DESC descDepth;
    ZeroMemory(&descDepth, sizeof(descDepth));
    descDepth.Width = inWidth;
    descDepth.Height = inHeight;
    descDepth.MipLevels = 1;
    descDepth.ArraySize = 1;
    descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    descDepth.SampleDesc.Count = 1;
    descDepth.Usage = D3D11_USAGE_DEFAULT;
    descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    HRESULT hr = mDev->CreateTexture2D(&descDepth, nullptr, pDepthTexture);

    D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
    ZeroMemory(&descDSV, sizeof(descDSV));
    descDSV.Format = descDepth.Format;
    descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    descDSV.Texture2D.MipSlice = 0;
    hr = mDev->CreateDepthStencilView(*pDepthTexture, &descDSV, pDepthView);
    return hr == 0;
}

ID3D11DepthStencilState *Graphics::CreateDepthStencilState(bool inDepthTestEnable, D3D11_COMPARISON_FUNC inDepthComparisonFunction)
{
    D3D11_DEPTH_STENCIL_DESC dsDesc;
    dsDesc.DepthEnable = true;
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    dsDesc.DepthFunc = inDepthComparisonFunction;
    dsDesc.StencilEnable = false;
    dsDesc.StencilReadMask = 0xFF;
    dsDesc.StencilWriteMask = 0xFF;
    dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
    dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    ID3D11DepthStencilState* depthState = nullptr;
    HRESULT hr = mDev->CreateDepthStencilState(&dsDesc, &depthState);

    return depthState;
}


ID3D11Buffer* Graphics::CreateGraphicsBuffer(const void *initialData, int inDataSize, D3D11_BIND_FLAG inBindFlags, D3D11_CPU_ACCESS_FLAG inCPUAccessFlags, D3D11_USAGE inUsage)
{
    D3D11_BUFFER_DESC desc;
    ZeroMemory(&desc, sizeof(desc));
    desc.Usage = inUsage;
    desc.ByteWidth = inDataSize;
    desc.BindFlags = inBindFlags;
    desc.CPUAccessFlags = inCPUAccessFlags;
    ID3D11Buffer* pBuffer;
    GetDevice()->CreateBuffer(&desc, nullptr, &pBuffer);
    if (nullptr != initialData)
        UploadBuffer(pBuffer, initialData, inDataSize);
    return pBuffer;
}

void Graphics::UploadBuffer(ID3D11Buffer* buffer, const void* data, size_t dataSize)
{
    D3D11_MAPPED_SUBRESOURCE map;
    GetDeviceContext()->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &map);
    memcpy(map.pData, data, dataSize);
    GetDeviceContext()->Unmap(buffer, 0);
}

void Graphics::SetActiveTexture(int slot, ID3D11ShaderResourceView* pView)
{
    GetDeviceContext()->PSSetShaderResources(slot, 1, &pView);
}

void Graphics::SetActiveSampler(int slot, ID3D11SamplerState* pSampler)
{
    GetDeviceContext()->PSSetSamplers(slot, 1, &pSampler);
}

void Graphics::SetRenderTarget(ID3D11RenderTargetView * renderTarget, ID3D11DepthStencilView * depthView)
{
	currBuffer = renderTarget;
	currDepth = depthView;
	mDevcon->OMSetRenderTargets(1, &currBuffer, currDepth);
}

void Graphics::SetViewport(float x, float y, float width, float height)
{
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
	viewport.TopLeftX = x;
	viewport.TopLeftY = y;
	viewport.Width = width;
	viewport.Height = height;
	viewport.MaxDepth = 1.0f;
	viewport.MinDepth = 0.0f;
	mDevcon->RSSetViewports(1, &viewport);
}

ID3D11BlendState * Graphics::CreateBlendState(bool enable, D3D11_BLEND srcBlend, D3D11_BLEND dstBlend)
{
	ID3D11BlendState* state;
	D3D11_BLEND_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_BLEND_DESC));
	desc.AlphaToCoverageEnable = FALSE;
	desc.IndependentBlendEnable = FALSE;
	desc.RenderTarget[0].BlendEnable = enable;
	desc.RenderTarget[0].SrcBlend = srcBlend;
	desc.RenderTarget[0].DestBlend = dstBlend;
	desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
	desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_DEST_ALPHA;
	desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	mDev->CreateBlendState(&desc, &state);
	return state;
}

void Graphics::SetBlendState(ID3D11BlendState* inBlendState) {
	float blendFactor[4];

	// Setup the blend factor.
	blendFactor[0] = 0.0f;
	blendFactor[1] = 0.0f;
	blendFactor[2] = 0.0f;
	blendFactor[3] = 0.0f;
	mDevcon->OMSetBlendState(inBlendState, blendFactor, 0xffffffff);
}
