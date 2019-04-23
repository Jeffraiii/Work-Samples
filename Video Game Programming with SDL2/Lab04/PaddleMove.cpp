//
//  PaddleMove.cpp
//  Game-mac
//
//  Created by tml on 2/6/18.
//  Copyright © 2018 Sanjay Madhav. All rights reserved.
//

#include <stdio.h>
#include "PaddleMove.h"
#include "Actor.h"

PaddleMove::PaddleMove(class Actor* owner):MoveComponent(owner){
    
}

void PaddleMove::Update(float deltaTime){
    MoveComponent::Update(deltaTime);
    if (mOwner->GetPosition().x <= 32+52){
        mOwner->SetPosition(Vector2(32+52,768-20));
    }
    else if (mOwner->GetPosition().x >= 1024-32-52){
        mOwner->SetPosition(Vector2(1024-32-52,768-20));
    }
}
