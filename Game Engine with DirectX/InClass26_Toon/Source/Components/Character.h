#pragma once
#include "Component.h"
#include "Job.h"

class Animation;
class RenderObj;
class Skeleton;
class SkinnedObj;

class Character : public Component
{
public:
    Character(SkinnedObj* pObj);
    void LoadProperties(const rapidjson::Value& properties) override;
    bool SetAnim(const std::string& animName);
    void UpdateAnim(float deltaTime);
    void Update(float deltaTime) override;
    void DoAnimJob();

protected:
    class AnimJob : public JobManager::Job
    {
    public:
        AnimJob(Character* pChar)
            : mOwner(pChar)
        {}
        void DoIt() override;
    private:
        Character* mOwner;
    };

    SkinnedObj* mSkinnedObj;
    Skeleton* mSkeleton;
    std::unordered_map<std::string, const Animation*> mAnims;
    const Animation* mCurrentAnim;
    float mAnimationTime;
    AnimJob mAnimJob;
};