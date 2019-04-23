// We want to use row major matrices
#pragma pack_matrix(row_major)

cbuffer PerCameraConstants : register(b0)
{
    float4x4 c_viewProj;
    float3 c_cameraPosition;
};

cbuffer PerObjectConstants : register(b1)
{
    float4x4 c_modelToWorld;
};

struct DirLightData
{
    float3 color;
    float pad0;
    float3 dir;
    float pad1;
};

cbuffer LightingConstants : register(b2)
{
    float3	c_ambient;
    DirLightData c_dirLight;
};

SamplerState DefaultSampler : register(s0);
Texture2D DiffuseTexture : register(t0);
