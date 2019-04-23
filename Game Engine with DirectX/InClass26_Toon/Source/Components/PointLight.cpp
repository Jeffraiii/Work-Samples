#include "stdafx.h"
#include "PointLight.h"
#include "jsonUtil.h"
#include "RenderObj.h"

PointLight::PointLight(RenderObj* pObj)
    : Component(pObj)
{
    mLight = mObj->GetGame()->AllocateLight();
    mLight->position = pObj->mObjectData.c_modelToWorld.GetTranslation();
}

PointLight::~PointLight()
{
    mObj->GetGame()->FreeLight(mLight);
}

void PointLight::LoadProperties(const rapidjson::Value& properties)
{
    GetVectorFromJSON(properties, "diffuseColor", mLight->diffuseColor);
    GetVectorFromJSON(properties, "specularColor", mLight->specularColor);
    GetFloatFromJSON(properties, "specularPower", mLight->specularPower);
    GetFloatFromJSON(properties, "innerRadius", mLight->innerRadius);
    GetFloatFromJSON(properties, "outerRadius", mLight->outerRadius);
}
