#pragma once

class Graphics;

class VertexBuffer
{
public:
    VertexBuffer(Graphics *graphics,
        const void* vertexData, uint32_t vertexCount, uint32_t vertexStride,
        const void* indexData, uint32_t indexCount, uint32_t indexStride
        );
    ~VertexBuffer();
    void SetActive() const;
    void Draw() const;

private:
    Graphics* mGraphics;
    ID3D11Buffer* mVertexBuffer;
    ID3D11Buffer* mIndexBuffer;
    uint32_t mVertStride;
    uint32_t mIndexCount;
    DXGI_FORMAT mIndexFormat;
};