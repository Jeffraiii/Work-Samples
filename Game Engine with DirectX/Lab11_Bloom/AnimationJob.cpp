#include "stdafx.h"
#include "AnimationJob.h"
#include "Animation.h"
#include "Components/Character.h"
#include "Skeleton.h"
#include "SkinnedObj.h"


AnimationJob::AnimationJob(Character* pChar)
{
	mChar = pChar;
}


AnimationJob::~AnimationJob()
{
	delete mChar;
}

void AnimationJob::DoIt()
{
	while (mChar->mAnimationTime >= mChar->mCurrentAnim->GetLength())
	{
		mChar->mAnimationTime -= mChar->mCurrentAnim->GetLength();
	}
	std::vector<Matrix4> outPoses;
	mChar->mCurrentAnim->GetGlobalPoseAtTime(outPoses, mChar->mSkeleton, mChar->mAnimationTime);
	for (int i = 0; i < mChar->mSkeleton->GetNumBones(); ++i)
	{
		mChar->mSkinnedObj->mSkinConstants.c_skinMatrix[i] = mChar->mSkeleton->GetGlobalInvBindPoses()[i] * outPoses[i];
	}
}
