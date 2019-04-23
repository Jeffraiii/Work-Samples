//
//  PlaneMove.cpp
//  Game-mac
//
//  Created by tml on 3/7/18.
//  Copyright © 2018 Sanjay Madhav. All rights reserved.
//

#include <stdio.h>
#include "PlaneMove.h"
#include "Plane.h"
#include "Actor.h"
#include "Game.h"

PlaneMove::PlaneMove(class Actor* owner): MoveComponent(owner){
    this->SetForwardSpeed(200.0f);
    SetNextTile(mOwner->GetGame()->GetGrid()->GetStartTile()->GetParent());
}

void PlaneMove::Update(float deltaTime){
    MoveComponent::Update(deltaTime);
    if (mNextTile != nullptr && (mNextTile->GetPosition() - mOwner->GetPosition()).Length() < 2.0f){
        SetNextTile(mNextTile->GetParent());
    }
    if (mOwner->GetPosition().x > 1024){
        mOwner->SetState(Actor::EDead);
    }
}

void PlaneMove::SetNextTile(const Tile *n){
    mNextTile = n;
    if (mNextTile == nullptr){
        mOwner->SetRotation(0.0f);
    }
    else{
        Vector2 v = n->GetPosition() - mOwner->GetPosition();
        mOwner->SetRotation(atan2(-v.y, v.x));
    }
}
