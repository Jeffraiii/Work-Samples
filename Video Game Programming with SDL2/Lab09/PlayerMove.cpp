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

PlayerMove::PlayerMove(class Actor* owner):MoveComponent(owner){
    ChangeState(Falling);
    mZSpeed = 0.0f;
}

void PlayerMove::Update(float deltaTime){
    if (mCurrentState == Falling){
        UpdateFalling(deltaTime);
    }
    else if (mCurrentState == OnGround){
        UpdateOnGround(deltaTime);
    }
    else if (mCurrentState == Jump){
        UpdateJump(deltaTime);
    }
}

void PlayerMove::ProcessInput(const Uint8 *keyState){
    if (keyState[SDL_SCANCODE_W]){
        this->SetForwardSpeed(350.0f);
    }
    else if (keyState[SDL_SCANCODE_S]){
        this->SetForwardSpeed(-350.0f);
    }
    else{
        this->SetForwardSpeed(0.0f);
    }
    if (keyState[SDL_SCANCODE_S] && keyState[SDL_SCANCODE_W]){
        this->SetForwardSpeed(0.0f);
    }
    
    if (keyState[SDL_SCANCODE_D]){
        this->SetStrafeSpeed(350.0f);
    }
    else if (keyState[SDL_SCANCODE_A]){
        this->SetStrafeSpeed(-350.0f);
    }
    else{
        this->SetStrafeSpeed(0.0f);
    }
    if (keyState[SDL_SCANCODE_A] && keyState[SDL_SCANCODE_D]){
        this->SetStrafeSpeed(0.0f);
    }
    if (keyState[SDL_SCANCODE_SPACE] && mCurrentState == OnGround){
        mZSpeed = JumpSpeed;
        ChangeState(Jump);
    }
    
    int x, y;
    SDL_GetRelativeMouseState(&x, &y);
    this->SetAngularSpeed(((float)x/500.0f) * Math::Pi*10.0f);
    mOwner->GetCamera()->SetPitchSpeed(((float)y/500.0f) * Math::Pi*10.0f);
}

void PlayerMove::ChangeState(PlayerMove::MoveState state){
    mCurrentState = state;
}

void PlayerMove::UpdateJump(float deltaTime){
    MoveComponent::Update(deltaTime);
    mZSpeed += deltaTime * Gravity;
    mOwner->SetPosition(mOwner->GetPosition() + deltaTime * Vector3(0,0,mZSpeed));
    for (Block* b : mOwner->GetGame()->GetBlocks()){
        if (FixCollision(mOwner->GetCollision(), b->GetCollision()) == PlayerMove::Bottom){
            mZSpeed = 0.0f;
            ChangeState(OnGround);
        }
    }
    if (mZSpeed <= 0){
        ChangeState(Falling);
    }
}

void PlayerMove::UpdateFalling(float deltaTime){
    MoveComponent::Update(deltaTime);
    mZSpeed += deltaTime * Gravity;
    mOwner->SetPosition(mOwner->GetPosition() + deltaTime * Vector3(0,0,mZSpeed));
    for (Block* b : mOwner->GetGame()->GetBlocks()){
        if (FixCollision(mOwner->GetCollision(), b->GetCollision()) == PlayerMove::Top){
            mZSpeed = 0.0f;
            ChangeState(OnGround);
        }
    }
}

void PlayerMove::UpdateOnGround(float deltaTime){
    MoveComponent::Update(deltaTime);
    int count = 0;
    for (Block* b : mOwner->GetGame()->GetBlocks()){
        if (FixCollision(mOwner->GetCollision(), b->GetCollision()) == PlayerMove::Top){
            count++;
        }
    }
    if (count == 0){
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
            return PlayerMove::Side;
        }
        if (min == dy2){
            mOwner->SetPosition(Vector3(mOwner->GetPosition().x, mOwner->GetPosition().y+dy2,mOwner->GetPosition().z));
            return PlayerMove::Side;
        }
        if (min == dx1){
            mOwner->SetPosition(Vector3(mOwner->GetPosition().x - dx1, mOwner->GetPosition().y,mOwner->GetPosition().z));
            return PlayerMove::Side;
        }
        if (min == dx2){
            mOwner->SetPosition(Vector3(mOwner->GetPosition().x + dx2, mOwner->GetPosition().y,mOwner->GetPosition().z));
            return PlayerMove::Side;
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
