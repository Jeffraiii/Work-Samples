#include "MoveComponent.h"
#include "Actor.h"

MoveComponent::MoveComponent(class Actor* owner)
:Component(owner)
,mAngularSpeed(0.0f)
,mForwardSpeed(0.0f)
,mStrafeSpeed(0.0f)
{
	
}

void MoveComponent::Update(float deltaTime)
{
	// TODO: Implement in Part 3
    mOwner->SetRotation(mOwner->GetRotation()+mAngularSpeed*deltaTime);
    Vector3 velocity = mOwner->GetForward()*mForwardSpeed;
    mOwner->SetPosition(mOwner->GetPosition()+velocity*deltaTime);
    mOwner->SetPosition(mOwner->GetPosition() + mOwner->GetRight()* mStrafeSpeed *deltaTime);
}
