#pragma once
#include "BoneTransform.h"

class Game;
class Skeleton;

class Animation
{
public:
    Animation();
    uint32_t GetNumBones() const { return mNumBones; }
    uint32_t GetNumFrames() const { return mNumFrames; }
    float GetLength() const { return mLength; }
    static Animation* StaticLoad(const WCHAR* fileName, Game* pGame);
    bool Load(const WCHAR* fileName);
    void GetGlobalPoseAtTime(
        std::vector<Matrix4>& outPoses, const Skeleton *inSkeleton, float inTime
    ) const;

private:
    uint32_t mNumBones;
    uint32_t mNumFrames;
    float mLength;
    std::vector<std::vector<BoneTransform>> mTracks;
};