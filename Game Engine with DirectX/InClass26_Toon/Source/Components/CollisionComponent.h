#pragma once
#include "Component.h"
#include "Physics.h"

class CollisionComponent : public Component
{
public:
    CollisionComponent(RenderObj* pObj);
    ~CollisionComponent() override;
    void LoadProperties(const rapidjson::Value& properties) override;
    Physics::AABB GetAABB() const;

private:
    Physics::AABB mAABB;
};