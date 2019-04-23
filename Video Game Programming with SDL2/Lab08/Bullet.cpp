//
//  Bullet.cpp
//  Game-mac
//
//  Created by tml on 3/14/18.
//  Copyright © 2018 Sanjay Madhav. All rights reserved.
//

#include <stdio.h>
#include "Bullet.h"
#include "Game.h"
#include "Renderer.h"

Bullet::Bullet(class Game* game): Actor(game){
    mMovement = new MoveComponent(this);
    mMesh = new MeshComponent(this);
    mMesh->SetMesh(GetGame()->GetRenderer()->GetMesh("Assets/Sphere.gpmesh"));
    mCollision = new CollisionComponent(this);
    mCollision->SetSize(10.0f, 10.0f, 10.0f);
    SetScale(0.5f);
    mMovement->SetForwardSpeed(400.0f);
}

void Bullet::UpdateActor(float deltaTime){
    for (Block* b : GetGame()->GetBlocks()){
        if (mCollision->Intersect(b->GetCollision())){
            this->SetState(Actor::EDead);
        }
    }
    if (mCollision->Intersect(GetGame()->GetTank1()->GetCollision())){
        if (GetGame()->GetTank1() != mShooter){
            this->SetState(Actor::EDead);
            GetGame()->GetTank1()->Respawn();
        }
    }
    if (mCollision->Intersect(GetGame()->GetTank2()->GetCollision())){
        if (GetGame()->GetTank2() != mShooter){
            this->SetState(Actor::EDead);
            GetGame()->GetTank2()->Respawn();
        }
    }
}
