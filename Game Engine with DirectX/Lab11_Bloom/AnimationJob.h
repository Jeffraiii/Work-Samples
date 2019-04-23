#pragma once
#include "Job.h"

class Animation;
class RenderObj;
class Skeleton;
class SkinnedObj;
class Character;

class AnimationJob :
	public Job
{
public:
	AnimationJob(Character* pChar);
	~AnimationJob();
	void DoIt() override;
private:
	Character* mChar;
};

