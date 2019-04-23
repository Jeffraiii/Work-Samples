#pragma once
#include "engineMath.h"

class Graphics;

class Camera
{
public:
    struct PerCameraConstants
    {
        Matrix4 c_viewProj;
        Vector3 c_cameraPosition;
        float pad;
    };

    Camera(Graphics* pGraphics);
    ~Camera();
    void SetActive();
    void SetViewMat(const Matrix4& view) { mView = view; }
    void SetProjMat(const Matrix4& proj) { mProj = proj; }

    PerCameraConstants mCameraData;

private:
    Graphics* mGraphics;
    Matrix4 mView;
    Matrix4 mProj;
    ID3D11Buffer* mCameraBuffer;
};