#pragma once
#include "Component.h"
#include "../AnimationJob.h"

class Animation;
class RenderObj;
class Skeleton;
class SkinnedObj;

class Character : public Component
{
	friend AnimationJob;
public:
    Character(SkinnedObj* pObj);
    void LoadProperties(const rapidjson::Value& properties) override;
    bool SetAnim(const std::string& animName);
    void UpdateAnim(float deltaTime);
    void Update(float deltaTime) override;


protected:
    SkinnedObj* mSkinnedObj;
    Skeleton* mSkeleton;
    std::unordered_map<std::string, const Animation*> mAnims;
    const Animation* mCurrentAnim;
    float mAnimationTime;
};