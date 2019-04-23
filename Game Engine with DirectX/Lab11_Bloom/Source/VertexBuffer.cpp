#include "stdafx.h"
#include "VertexBuffer.h"
#include "Graphics.h"

VertexBuffer::VertexBuffer(Graphics *graphics,
    const void* vertexData, uint32_t vertexCount, uint32_t vertexStride,
    const void* indexData, uint32_t indexCount, uint32_t indexStride
    )
    : mGraphics(graphics)
    , mVertStride(vertexStride)
    , mIndexCount(indexCount)
{
    mVertexBuffer = mGraphics->CreateGraphicsBuffer(vertexData, vertexCount * vertexStride, D3D11_BIND_VERTEX_BUFFER, D3D11_CPU_ACCESS_WRITE, D3D11_USAGE_DYNAMIC);
    mIndexBuffer = mGraphics->CreateGraphicsBuffer(indexData, indexCount * indexStride, D3D11_BIND_INDEX_BUFFER, D3D11_CPU_ACCESS_WRITE, D3D11_USAGE_DYNAMIC);
    if (indexStride == sizeof(uint16_t))
        mIndexFormat = DXGI_FORMAT_R16_UINT;
    else if (indexStride == sizeof(uint8_t))
        mIndexFormat = DXGI_FORMAT_R8_UINT;
    else
        mIndexFormat = DXGI_FORMAT_R32_UINT;
}

VertexBuffer::~VertexBuffer()
{
    mVertexBuffer->Release();
    mIndexBuffer->Release();
}

void VertexBuffer::SetActive() const
{
    UINT zero = 0;
    mGraphics->GetDeviceContext()->IASetVertexBuffers(0, 1, &mVertexBuffer, &mVertStride, &zero);
    mGraphics->GetDeviceContext()->IASetIndexBuffer(mIndexBuffer, mIndexFormat, 0);
}

void VertexBuffer::Draw() const
{
    mGraphics->GetDeviceContext()->DrawIndexed(mIndexCount, 0, 0);
}

