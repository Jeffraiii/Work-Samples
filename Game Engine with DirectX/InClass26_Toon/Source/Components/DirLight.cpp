#include "stdafx.h"
#include "DirLight.h"
#include "jsonUtil.h"
#include "RenderObj.h"

DirLight::DirLight(RenderObj* pObj)
    : Component(pObj)
{
    mLight = mObj->GetGame()->AllocateDirLight();
}

DirLight::~DirLight()
{
    mObj->GetGame()->FreeDirLight(mLight);
}

void DirLight::LoadProperties(const rapidjson::Value& properties)
{
    GetVectorFromJSON(properties, "color", mLight->color);
    if (GetVectorFromJSON(properties, "direction", mLight->dir))
        mLight->dir.Normalize();
}
