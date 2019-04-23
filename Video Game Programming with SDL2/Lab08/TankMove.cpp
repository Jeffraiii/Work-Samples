//
//  TankMove.cpp
//  Game-mac
//
//  Created by tml on 3/14/18.
//  Copyright © 2018 Sanjay Madhav. All rights reserved.
//

#include <stdio.h>
#include "TankMove.h"
#include "Actor.h"
#include "Game.h"

TankMove::TankMove(class Actor* owner):MoveComponent(owner){
    mForwardKey = SDL_SCANCODE_W;
    mBackKey = SDL_SCANCODE_S;
    mLeftKey = SDL_SCANCODE_A;
    mRightKey = SDL_SCANCODE_D;
}

void TankMove::Update(float deltaTime){
    MoveComponent::Update(deltaTime);
    for (Block* a : mOwner->GetGame()->GetBlocks()){
        if (a->GetCollision()->Intersect(mOwner->GetCollision())){
            float dx1, dx2, dy1, dy2;
            float min = 0;
            dy2 = abs(a->GetCollision()->GetMax().y - mOwner->GetCollision()->GetMin().y);
            dy1 = abs(a->GetCollision()->GetMin().y - mOwner->GetCollision()->GetMax().y);
            dx1 = abs(a->GetCollision()->GetMin().x - mOwner->GetCollision()->GetMax().x);
            dx2 = abs(a->GetCollision()->GetMax().x - mOwner->GetCollision()->GetMin().x);
            min = Math::Min(dy1, Math::Min(dy2, Math::Min(dx1, dx2)));
            if (min == dy1){
                mOwner->SetPosition(Vector3(mOwner->GetPosition().x, mOwner->GetPosition().y-dy1,0));
            }
            if (min == dy2){
                mOwner->SetPosition(Vector3(mOwner->GetPosition().x, mOwner->GetPosition().y+dy2,0));
            }
            if (min == dx1){
                mOwner->SetPosition(Vector3(mOwner->GetPosition().x - dx1, mOwner->GetPosition().y,0));
            }
            if (min == dx2){
                mOwner->SetPosition(Vector3(mOwner->GetPosition().x + dx2, mOwner->GetPosition().y,0));
            }
        }
    }
    
}

void TankMove::ProcessInput(const Uint8 *keyState){
    if (keyState[mForwardKey]){
        SetForwardSpeed(250);
    }
    else if (keyState[mBackKey]){
        SetForwardSpeed(-250);
    }
    else{
        SetForwardSpeed(0);
    }
    if (keyState[mLeftKey]){
        SetAngularSpeed(Math::TwoPi);
    }
    else if (keyState[mRightKey]){
        SetAngularSpeed(-Math::TwoPi);
    }
    else{
        SetAngularSpeed(0);
    }
}

void TankMove::SetPlayerTwo(){
    mForwardKey = SDL_SCANCODE_O;
    mBackKey = SDL_SCANCODE_L;
    mLeftKey = SDL_SCANCODE_K;
    mRightKey = SDL_SCANCODE_SEMICOLON;
}
