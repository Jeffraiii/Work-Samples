#include "stdafx.h"
#include "CollisionComponent.h"
#include "game.h"
#include "jsonUtil.h"
#include "RenderObj.h"


CollisionComponent::CollisionComponent(RenderObj* pObj)
    : Component(pObj)
{
    mObj->GetGame()->mPhysics.AddObj(this);
}

CollisionComponent::~CollisionComponent()
{
    mObj->GetGame()->mPhysics.RemoveObj(this);
}

void CollisionComponent::LoadProperties(const rapidjson::Value& properties)
{
    GetVectorFromJSON(properties, "min", mAABB.mMin);
    GetVectorFromJSON(properties, "max", mAABB.mMax);
}

Physics::AABB CollisionComponent::GetAABB() const
{
    Physics::AABB aabb = mAABB;
    float scale = this->mObj->mObjectData.c_modelToWorld.GetScale().x;
    Vector3 pos = mObj->mObjectData.c_modelToWorld.GetTranslation();
    aabb.mMin *= scale;
    aabb.mMax *= scale;
    aabb.mMin += pos;
    aabb.mMax += pos;
    return aabb;
}