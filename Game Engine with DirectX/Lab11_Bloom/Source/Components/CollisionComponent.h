#pragma once
#include "Component.h"
#include "Physics.h"
class CollisionComponent : public Component
{
public:
	CollisionComponent(RenderObj* pObj);
	~CollisionComponent();
	Physics::AABB mAABB;
	void LoadProperties(const rapidjson::Value& properties) override;
	Physics::AABB GetAABB() const;
};

