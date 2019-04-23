#include "stdafx.h"
#include "SimpleRotate.h"
#include "jsonUtil.h"
#include "RenderObj.h"

SimpleRotate::SimpleRotate(RenderObj * pObj): Component(pObj)
{
}

SimpleRotate::~SimpleRotate()
{
}

void SimpleRotate::LoadProperties(const rapidjson::Value & properties)
{
	GetFloatFromJSON(properties, "speed", mSpeed);
}

void SimpleRotate::Update(float deltaTime)
{
	Matrix4 rot = mObj->mObjectData.c_modelToWorld.CreateRotationZ(mSpeed * deltaTime);
	mObj->mObjectData.c_modelToWorld *= rot;
}
