//
//  PlayerMove.cpp
//  Game-mac
//
//  Created by tml on 3/22/18.
//  Copyright © 2018 Sanjay Madhav. All rights reserved.
//

#include <stdio.h>
#include "PlayerMove.h"
#include "Actor.h"
#include "Game.h"
#include "SDL/SDL.h"
#include "Player.h"
#include <iostream>

PlayerMove::PlayerMove(class Actor* owner):MoveComponent(owner){
    ChangeState(Falling);
    mMass = 1.0f;
    mGravity = Vector3(0.0f, 0.0f, -980.0f);
    mJumpForce = Vector3(0.0f, 0.0f, 35000.0f);
    mClimbForce = Vector3(0.0f, 0.0f, 1800.0f);
    mWallRunForce = Vector3(0.0f, 0.0f, 1200.0f);
    mWallClimbTimer = 0.0f;
    mSpacePressed = false;
    mRunningSFX = Mix_PlayChannel(-1, mOwner->GetGame()->GetSound("Assets/Sounds/Running.wav"), -1);
    Mix_Pause(mRunningSFX);
}

PlayerMove::~PlayerMove(){
    Mix_HaltChannel(mRunningSFX);
}

void PlayerMove::Update(float deltaTime){
    if (mCurrentState == Falling){
        Mix_Pause(mRunningSFX);
        UpdateFalling(deltaTime);
    }
    else if (mCurrentState == OnGround){
        if (mVelocity.Length() > 50.0f){
            Mix_Resume(mRunningSFX);
        }
        else{
            Mix_Pause(mRunningSFX);
        }
        UpdateOnGround(deltaTime);
    }
    else if (mCurrentState == Jump){
        Mix_Pause(mRunningSFX);
        UpdateJump(deltaTime);
    }
    else if (mCurrentState == WallClimb){
        Mix_Resume(mRunningSFX);
        UpdateWallClimb(deltaTime);
    }
    else if (mCurrentState == WallRun){
        Mix_Resume(mRunningSFX);
        UpdateWallRun(deltaTime);
    }
    if (mOwner->GetPosition().z < -750.0f){
        mOwner->SetPosition(((Player*) mOwner)->GetRespawnPos());
        mOwner->SetRotation(0.0f);
        mVelocity = Vector3::Zero;
        mPendingForces = Vector3::Zero;
        ChangeState(Falling);
    }
}

void PlayerMove::ProcessInput(const Uint8 *keyState){
    if (keyState[SDL_SCANCODE_W]){
        AddForce(700.0f * mOwner->GetForward());
    }
    if (keyState[SDL_SCANCODE_S]){
        AddForce(-700.0f * mOwner->GetForward());
    }
//    else{
//        AddForce(Vector3::Zero);
//    }
//    if (keyState[SDL_SCANCODE_S] && keyState[SDL_SCANCODE_W]){
//        AddForce(Vector3::Zero);
//    }
    
    if (keyState[SDL_SCANCODE_D]){
        AddForce(700.0f * mOwner->GetRight());
    }
    if (keyState[SDL_SCANCODE_A]){
        AddForce(-700.0f * mOwner->GetRight());
    }
//    else{
//        AddForce(Vector3::Zero);
//    }
//    if (keyState[SDL_SCANCODE_A] && keyState[SDL_SCANCODE_D]){
//        AddForce(Vector3::Zero);
//    }
    if (keyState[SDL_SCANCODE_SPACE] && mCurrentState == OnGround && !mSpacePressed){
        AddForce(mJumpForce);
        Mix_PlayChannel(-1, mOwner->GetGame()->GetSound("Assets/Sounds/Jump.wav"), 0);
        ChangeState(Jump);
    }
    mSpacePressed = keyState[SDL_SCANCODE_SPACE];
    int x, y;
    SDL_GetRelativeMouseState(&x, &y);
    this->SetAngularSpeed(((float)x/500.0f) * Math::Pi*10.0f);
    mOwner->GetCamera()->SetPitchSpeed(((float)y/500.0f) * Math::Pi*10.0f);
}

void PlayerMove::ChangeState(PlayerMove::MoveState state){
    mCurrentState = state;
}

void PlayerMove::UpdateJump(float deltaTime){
    AddForce(mGravity);
    PhysicsUpdate(deltaTime);
//    mZSpeed += deltaTime * Gravity;
//    mOwner->SetPosition(mOwner->GetPosition() + deltaTime * Vector3(0,0,mZSpeed));
    for (Block* b : mOwner->GetGame()->GetBlocks()){
        if (FixCollision(mOwner->GetCollision(), b->GetCollision()) == PlayerMove::Bottom){
            mVelocity.z = 0.0f;
            ChangeState(OnGround);
        }
        if (FixCollision(mOwner->GetCollision(), b->GetCollision()) == PlayerMove::SideX1){
            if (CanWallClimb(SideX1)){
                ChangeState(WallClimb);
                mWallClimbTimer = 0.0f;
                return;
            }
            else if (CanWallRun(SideX1)){
                ChangeState(WallRun);
                mWallRunTimer = 0.0f;
                return;
            }
        }
        if (FixCollision(mOwner->GetCollision(), b->GetCollision()) == PlayerMove::SideX2){
            if (CanWallClimb(SideX2)){
                ChangeState(WallClimb);
                mWallClimbTimer = 0.0f;
                return;
            }
            else if (CanWallRun(SideX2)){
                ChangeState(WallRun);
                mWallRunTimer = 0.0f;
                return;
            }
        }
        if (FixCollision(mOwner->GetCollision(), b->GetCollision()) == PlayerMove::SideY1){
            if (CanWallClimb(SideY1)){
                ChangeState(WallClimb);
                mWallClimbTimer = 0.0f;
                return;
            }
            else if (CanWallRun(SideY1)){
                ChangeState(WallRun);
                mWallRunTimer = 0.0f;
                return;
            }
        }
        if (FixCollision(mOwner->GetCollision(), b->GetCollision()) == PlayerMove::SideY2){
            if (CanWallClimb(SideY2)){
                ChangeState(WallClimb);
                mWallClimbTimer = 0.0f;
                return;
            }
            else if (CanWallRun(SideY2)){
                ChangeState(WallRun);
                mWallRunTimer = 0.0f;
                return;
            }
        }
    }
    if (mVelocity.z <= 0){
        ChangeState(Falling);
    }
}

void PlayerMove::UpdateFalling(float deltaTime){
    AddForce(mGravity);
    PhysicsUpdate(deltaTime);
//    mZSpeed += deltaTime * Gravity;
//    mOwner->SetPosition(mOwner->GetPosition() + deltaTime * Vector3(0,0,mZSpeed));
    for (Block* b : mOwner->GetGame()->GetBlocks()){
        if (FixCollision(mOwner->GetCollision(), b->GetCollision()) == PlayerMove::Top){
            mVelocity.z = 0.0f;
            Mix_PlayChannel(-1, mOwner->GetGame()->GetSound("Assets/Sounds/Land.wav"), 0);
            ChangeState(OnGround);
        }
    }
}

void PlayerMove::UpdateOnGround(float deltaTime){
    PhysicsUpdate(deltaTime);
    int count = 0;
    for (Block* b : mOwner->GetGame()->GetBlocks()){
        if (FixCollision(mOwner->GetCollision(), b->GetCollision()) == PlayerMove::Top){
            count++;
        }
        if (FixCollision(mOwner->GetCollision(), b->GetCollision()) == PlayerMove::SideX1){
            if (CanWallClimb(SideX1)){
                ChangeState(WallClimb);
                mWallClimbTimer = 0.0f;
                return;
            }
        }
        if (FixCollision(mOwner->GetCollision(), b->GetCollision()) == PlayerMove::SideX2){
            if (CanWallClimb(SideX2)){
                ChangeState(WallClimb);
                mWallClimbTimer = 0.0f;
                return;
            }
        }
        if (FixCollision(mOwner->GetCollision(), b->GetCollision()) == PlayerMove::SideY1){
            if (CanWallClimb(SideY1)){
                ChangeState(WallClimb);
                mWallClimbTimer = 0.0f;
                return;
            }
        }
        if (FixCollision(mOwner->GetCollision(), b->GetCollision()) == PlayerMove::SideY2){
            if (CanWallClimb(SideY2)){
                ChangeState(WallClimb);
                mWallClimbTimer = 0.0f;
                return;
            }
        }
    }
    if (count == 0){
        ChangeState(Falling);
    }
    
}

void PlayerMove::UpdateWallClimb(float deltaTime){
    mWallClimbTimer += deltaTime;
    AddForce(mGravity);
    if (mWallClimbTimer < 0.4f){
        AddForce(mClimbForce);
    }
    PhysicsUpdate(deltaTime);
    int count = 0;
    for (Block* b : mOwner->GetGame()->GetBlocks()){
        CollSide cs = FixCollision(mOwner->GetCollision(), b->GetCollision());
        bool x1 =  cs == PlayerMove::SideX1;
        bool x2 = cs == PlayerMove::SideX2;
        bool y1 = cs == PlayerMove::SideY1;
        bool y2 = cs == PlayerMove::SideY2;
        if (x1 || x2 || y1 || y2){
            count++;
        }
    }
    if (count == 0  || mVelocity.z <= 0.0f){
        mVelocity.z = 0.0f;
        ChangeState(Falling);
    }
}

void PlayerMove::UpdateWallRun(float deltaTime){
    mWallRunTimer += deltaTime;
    AddForce(mGravity);
    if (mWallRunTimer < 0.4f){
        AddForce(mWallRunForce);
    }
    PhysicsUpdate(deltaTime);
    for (Block* b : mOwner->GetGame()->GetBlocks()){
        FixCollision(mOwner->GetCollision(), b->GetCollision());
    }
    if (mVelocity.z <= 0.0f){
        mVelocity.z = 0.0f;
        ChangeState(Falling);
    }
}

PlayerMove::CollSide PlayerMove::FixCollision(class CollisionComponent *self, class CollisionComponent *block){
    if (block->Intersect(mOwner->GetCollision())){
        float dx1, dx2, dy1, dy2, dz1, dz2;
        float min = 0;
        dy2 = abs(block->GetMax().y - mOwner->GetCollision()->GetMin().y);
        dy1 = abs(block->GetMin().y - mOwner->GetCollision()->GetMax().y);
        dx1 = abs(block->GetMin().x - mOwner->GetCollision()->GetMax().x);
        dx2 = abs(block->GetMax().x - mOwner->GetCollision()->GetMin().x);
        dz1 = abs(block->GetMin().z - mOwner->GetCollision()->GetMax().z);
        dz2 = abs(block->GetMax().z - mOwner->GetCollision()->GetMin().z);
        min = Math::Min(dx1, Math::Min(dx2, Math::Min(dy1, Math::Min(dy2, Math::Min(dz1, dz2)))));
        if (min == dy1){
            mOwner->SetPosition(Vector3(mOwner->GetPosition().x, mOwner->GetPosition().y-dy1,mOwner->GetPosition().z));
            AddForce(Vector3(0.0f, -700.0f, 0.0f));
            return PlayerMove::SideY1;
        }
        if (min == dy2){
            mOwner->SetPosition(Vector3(mOwner->GetPosition().x, mOwner->GetPosition().y+dy2,mOwner->GetPosition().z));
            AddForce(Vector3(0.0f, 700.0f, 0.0f));
            return PlayerMove::SideY2;
        }
        if (min == dx1){
            mOwner->SetPosition(Vector3(mOwner->GetPosition().x - dx1, mOwner->GetPosition().y,mOwner->GetPosition().z));
            AddForce(Vector3(-700.0f, 0.0f, 0.0f));
            return PlayerMove::SideX1;
        }
        if (min == dx2){
            mOwner->SetPosition(Vector3(mOwner->GetPosition().x + dx2, mOwner->GetPosition().y,mOwner->GetPosition().z));
            AddForce(Vector3(700.0f, 0.0f, 0.0f));
            return PlayerMove::SideX2;
        }
        if (min == dz1){
            mOwner->SetPosition(Vector3(mOwner->GetPosition().x, mOwner->GetPosition().y,mOwner->GetPosition().z - dz1));
            return PlayerMove::Bottom;
        }
        if (min == dz2){
            mOwner->SetPosition(Vector3(mOwner->GetPosition().x, mOwner->GetPosition().y,mOwner->GetPosition().z + dz2));
            return PlayerMove::Top;
        }
        
    }
    return PlayerMove::None;
}

void PlayerMove::PhysicsUpdate(float deltaTime){
    mAcceleration = mPendingForces * (1.0f/mMass);
    mVelocity += deltaTime * mAcceleration;
    FixXYVelocity();
    mOwner->SetPosition(mOwner->GetPosition() + deltaTime * mVelocity);
    mOwner->SetRotation(mOwner->GetRotation()+GetAngularSpeed()*deltaTime);
    mPendingForces = Vector3::Zero;
}

void PlayerMove::FixXYVelocity(){
    Vector2 xyVelocity = Vector2(mVelocity.x, mVelocity.y);
    if (xyVelocity.Length() > 400.0f){
        xyVelocity.Normalize();
        xyVelocity *= 400.0f;
    }
    if (mCurrentState == OnGround || mCurrentState == WallClimb){
        if (Math::NearZero(mAcceleration.x)){
            xyVelocity.x *= 0.9f;
        }
        if (mAcceleration.x * xyVelocity.x < 0){
            xyVelocity.x *= 0.9f;
        }
        if (Math::NearZero(mAcceleration.y)){
            xyVelocity.y *= 0.9f;
        }
        if (mAcceleration.y * xyVelocity.y < 0){
            xyVelocity.y *= 0.9f;
        }
    }
    mVelocity.x = xyVelocity.x;
    mVelocity.y = xyVelocity.y;
}

bool PlayerMove::CanWallClimb(PlayerMove::CollSide cs){
    Vector3 normal;
    if (cs == SideX1){
        normal = Vector3(-1,0,0);
    }
    else if (cs == SideX2){
        normal = Vector3(1, 0, 0);
    }
    else if (cs == SideY1){
        normal = Vector3(0, -1, 0);
    }
    else if (cs == SideY2){
        normal = Vector3(0, 1, 0);
    }
    if (Vector3::Dot(mOwner->GetForward(), normal) >= -1 && Vector3::Dot(mOwner->GetForward(), normal) <= -0.8f){
        Vector3 temp = mVelocity;
        temp.Normalize();
        if (Vector3::Dot(temp, normal) >= -1 && Vector3::Dot(temp, normal) <= -0.8f){
            if (mVelocity.Length() > 350.0f){
                return true;
            }
        }
    }
    return false;
}

bool PlayerMove::CanWallRun(PlayerMove::CollSide cs){
    Vector3 normal;
    if (cs == SideX1){
        normal = Vector3(-1,0,0);
    }
    else if (cs == SideX2){
        normal = Vector3(1, 0, 0);
    }
    else if (cs == SideY1){
        normal = Vector3(0, -1, 0);
    }
    else if (cs == SideY2){
        normal = Vector3(0, 1, 0);
    }
    if (Vector3::Dot(mOwner->GetForward(), normal) >= -0.5f && Vector3::Dot(mOwner->GetForward(), normal) <= 0.5f){
        Vector3 temp = mVelocity;
        temp.Normalize();
        if (Vector3::Dot(temp, normal) >= -0.5f && Vector3::Dot(temp, normal) <= 0.5f){
            if (mVelocity.Length() > 350.0f){
                return true;
            }
        }
    }
    return false;
}
