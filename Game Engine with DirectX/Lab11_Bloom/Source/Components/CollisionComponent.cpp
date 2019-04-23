#include "stdafx.h"
#include "CollisionComponent.h"
#include "jsonUtil.h"
#include "RenderObj.h"
#include "game.h"


CollisionComponent::CollisionComponent(RenderObj* pObj) : Component(pObj)
{
	mObj = pObj;
	mObj->GetGame()->mPhysics->AddObj(this);
}


CollisionComponent::~CollisionComponent()
{
	mObj->GetGame()->mPhysics->RemoveObj(this);

}

void CollisionComponent::LoadProperties(const rapidjson::Value & properties)
{
	GetVectorFromJSON(properties, "min", mAABB.mMin);
	GetVectorFromJSON(properties, "max", mAABB.mMax);
}

Physics::AABB CollisionComponent::GetAABB() const
{
	Vector3 tempMax = mAABB.mMax;
	Vector3 tempMin = mAABB.mMin;
	tempMax = tempMax * mObj->mObjectData.c_modelToWorld.GetScale().x;
	tempMin = tempMin * mObj->mObjectData.c_modelToWorld.GetScale().x;
	tempMax = tempMax + mObj->mObjectData.c_modelToWorld.GetTranslation();
	tempMin = tempMin + mObj->mObjectData.c_modelToWorld.GetTranslation();
	return Physics::AABB(tempMin, tempMax);
}
