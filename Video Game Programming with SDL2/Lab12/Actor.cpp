#include "Actor.h"
#include "Game.h"
#include "Component.h"
#include <algorithm>

Actor::Actor(Game* game)
	:mGame(game)
	,mState(EActive)
	,mPosition(Vector3::Zero)
	,mScale(1.0f)
	,mRotation(0.0f)
{
	// TODO
    mGame->AddActor(this);
    mMovement = nullptr;
    mCollision = nullptr;
    mMesh = nullptr;
    mCamera = nullptr;
}

Actor::~Actor()
{
	// TODO
    mGame->RemoveActor(this);
    delete mMovement;
    delete mCollision;
    delete mMesh;
    delete mCamera;
}

Vector3 Actor::GetForward() const
{
    return Vector3(Math::Cos(mRotation), Math::Sin(mRotation), 0.0f);
}

void Actor::Update(float deltaTime)
{
	// TODO
    if (mState == EActive){
        //update components
        if (mMovement != nullptr){
            mMovement->Update(deltaTime);
        }
        if (mCollision != nullptr){
            mCollision->Update(deltaTime);
        }
        if (mMesh != nullptr){
            mMesh->Update(deltaTime);
        }
        if (mCamera != nullptr){
            mCamera->Update(deltaTime);
        }
        UpdateActor(deltaTime);
        Matrix4 scaleM = Matrix4::CreateScale(mScale);
        Matrix4 rotationM = Matrix4::CreateRotationZ(mRotation);
        Matrix4 positionM = Matrix4::CreateTranslation(mPosition);
        mWorldTransform = scaleM * rotationM * Matrix4::CreateFromQuaternion(mQuat) * positionM;
    }
}

void Actor::UpdateActor(float deltaTime)
{
}

void Actor::ProcessInput(const Uint8* keyState)
{
	// TODO
    if (mState == EActive){
        //ProcessInput on all components
        if (mMovement != nullptr){
            mMovement->ProcessInput(keyState);
        }
        if (mCollision != nullptr){
            mCollision->ProcessInput(keyState);
        }
        if (mMesh != nullptr){
            mMesh->ProcessInput(keyState);
        }
        if (mCamera != nullptr){
            mCamera->ProcessInput(keyState);
        }
        ActorInput(keyState);
    }
}

void Actor::ActorInput(const Uint8* keyState)
{
}

Vector3 Actor::GetRight(){
    return Vector3(Math::Cos(mRotation + Math::PiOver2), Math::Sin(mRotation + Math::PiOver2), 0.0f);
}
