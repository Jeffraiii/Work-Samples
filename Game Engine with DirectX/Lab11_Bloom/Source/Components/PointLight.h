#pragma once
#include "Component.h"
#include "game.h"

class PointLight : public Component
{
public:
    PointLight(RenderObj* pObj);
    ~PointLight() override;
    void LoadProperties(const rapidjson::Value& properties) override;

private:
    Game::PointLightData* mLight;
};
