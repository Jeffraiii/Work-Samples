#pragma once
#include "BoneTransform.h"

class Game;

class Skeleton
{
public:
    struct Bone
    {
        BoneTransform mLocalBindPose;
        std::string mName;
        int mParent;
    };

    Skeleton(Game* pGame);
    size_t GetNumBones() const { return mBones.size(); }
    const Bone& GetBone(size_t idx) const { return mBones[idx]; }
    const std::vector<Bone>& GetBones() const { return mBones; }
    const std::vector<Matrix4>& GetGlobalInvBindPoses() const { return mGlobalInvBindPoses; }
    static Skeleton* StaticLoad(const WCHAR* fileName, Game* pGame);
    bool Load(const WCHAR* fileName);

private:
    void ComputeGlobalInvBindPose();

    Game* mGame;
    std::vector<Bone> mBones;
    std::vector<Matrix4> mGlobalInvBindPoses;
};