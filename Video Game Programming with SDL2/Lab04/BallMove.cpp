//
//  BallMove.cpp
//  Game-mac
//
//  Created by tml on 2/6/18.
//  Copyright © 2018 Sanjay Madhav. All rights reserved.
//

#include <stdio.h>
#include "BallMove.h"
#include "Actor.h"
#include "Game.h"

BallMove::BallMove(class Actor* owner):MoveComponent(owner){
    mVelocity = Vector2(100,-100);
}

void BallMove::Update(float deltaTime){
    mOwner->SetPosition(mOwner->GetPosition()+mVelocity*deltaTime);
    if (mOwner->GetPosition().x <= 32+mOwner->GetCollision()->GetWidth()){
        mOwner->SetPosition(Vector2(32+mOwner->GetCollision()->GetWidth(), mOwner->GetPosition().y));
        mVelocity = Vector2(-mVelocity.x,mVelocity.y);
    }
    else if (mOwner->GetPosition().x >= 1024-32-mOwner->GetCollision()->GetWidth()){
        mOwner->SetPosition(Vector2(1024-32-mOwner->GetCollision()->GetWidth(),mOwner->GetPosition().y));
        mVelocity = Vector2(-mVelocity.x,mVelocity.y);
    }
    else if (mOwner->GetPosition().y <= 32+mOwner->GetCollision()->GetHeight()){
        mOwner->SetPosition(Vector2(mOwner->GetPosition().x,32+mOwner->GetCollision()->GetHeight()));
        mVelocity = Vector2(mVelocity.x, -mVelocity.y);
    }
    else if (mOwner->GetPosition().y >= 768-20){
        mOwner->SetPosition(Vector2(512, 648));
        mVelocity = Vector2(100, -100);
    }
    if (mOwner->GetCollision()->Intersect(mOwner->GetGame()->GetPaddle()->GetCollision())){
        mOwner->SetPosition(Vector2(mOwner->GetPosition().x, mOwner->GetGame()->GetPaddle()->GetPosition().y - mOwner->GetGame()->GetPaddle()->GetCollision()->GetHeight()/2.0f - mOwner->GetCollision()->GetHeight()/2.0f - 20));
        if (mOwner->GetPosition().x >= mOwner->GetGame()->GetPaddle()->GetPosition().x - mOwner->GetGame()->GetPaddle()->GetCollision()->GetWidth() && mOwner->GetPosition().x <= mOwner->GetGame()->GetPaddle()->GetPosition().x - mOwner->GetGame()->GetPaddle()->GetCollision()->GetWidth()/6.0f){
            Vector2 n = Vector2(-1, -6);
            n.Normalize();
            mVelocity = Vector2::Reflect(mVelocity, n);
        }
        else if (mOwner->GetPosition().x <= mOwner->GetGame()->GetPaddle()->GetPosition().x + mOwner->GetGame()->GetPaddle()->GetCollision()->GetWidth() && mOwner->GetPosition().x >= mOwner->GetGame()->GetPaddle()->GetPosition().x + mOwner->GetGame()->GetPaddle()->GetCollision()->GetWidth()/6.0f){
            Vector2 n = Vector2(1, -6);
            n.Normalize();
            mVelocity = Vector2::Reflect(mVelocity, n);
        }
        else if (mOwner->GetPosition().x <= mOwner->GetGame()->GetPaddle()->GetPosition().x + mOwner->GetGame()->GetPaddle()->GetCollision()->GetWidth()/6.0f && mOwner->GetPosition().x >= mOwner->GetGame()->GetPaddle()->GetPosition().x - mOwner->GetGame()->GetPaddle()->GetCollision()->GetWidth()/6.0f){
            mVelocity = Vector2::Reflect(mVelocity, Vector2(0, -1));
        }
    }
    for (Block* a : *(mOwner->GetGame()->GetBlocks())){
        if (mOwner->GetCollision()->Intersect(a->GetCollision())){
            Vector2 distance = mOwner->GetPosition() - a->GetPosition();
            float angle = atan2(distance.y, distance.x);
            //right
            if ((angle < atan2(16, 32) && angle >= Math::ToRadians(0)) || (angle <= Math::ToRadians(0) && angle > atan2(-16, 32))){
//                mOwner->SetPosition(Vector2(a->GetPosition().x + a->GetCollision()->GetWidth() + mOwner->GetCollision()->GetWidth(), mOwner->GetPosition().y));
                mVelocity = Vector2::Reflect(mVelocity, Vector2(1, 0));
            }
            //bottom
            else if (angle >= atan2(16, 32) && angle <= Math::Pi - atan2(16, 32)){
//                mOwner->SetPosition(Vector2(mOwner->GetPosition().x, a->GetPosition().y + a->GetCollision()->GetHeight() + mOwner->GetCollision()->GetHeight()));
                mVelocity = Vector2::Reflect(mVelocity, Vector2(0, 1));
            }
            else if ((angle > Math::Pi - atan2(16, 32) && angle <= Math::ToRadians(180)) || (angle < -Math::Pi + atan2(16, 32) && angle >= Math::ToRadians(-180))){
//                mOwner->SetPosition(Vector2(a->GetPosition().x - a->GetCollision()->GetWidth() - mOwner->GetCollision()->GetWidth(), mOwner->GetPosition().y));
                mVelocity = Vector2::Reflect(mVelocity, Vector2(-1, 0));
            }
            else if (angle >= -Math::Pi + atan2(16, 32) && angle <= atan2(-16, 32)){
//                mOwner->SetPosition(Vector2(mOwner->GetPosition().x, a->GetPosition().y - a->GetCollision()->GetHeight() - mOwner->GetCollision()->GetHeight()));
                mVelocity = Vector2::Reflect(mVelocity, Vector2(0, -1));
            }
            a->SetState(a->EDead);
        }
    }
}
