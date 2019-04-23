#pragma once
#include "Component.h"
#include "game.h"

class DirLight : public Component
{
public:
    DirLight(RenderObj* pObj);
    ~DirLight() override;
    void LoadProperties(const rapidjson::Value& properties) override;

private:
    Game::DirLightData* mLight;
};
