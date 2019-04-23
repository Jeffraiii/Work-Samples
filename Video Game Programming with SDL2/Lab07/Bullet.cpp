//
//  Bullet.cpp
//  Game-mac
//
//  Created by tml on 3/7/18.
//  Copyright © 2018 Sanjay Madhav. All rights reserved.
//

#include <stdio.h>
#include "Bullet.h"
#include "Game.h"

Bullet::Bullet(class Game* game):Actor(game){
    mSprite = new SpriteComponent(this, 200);
    mSprite->SetTexture(GetGame()->GetTexture("Assets/Bullet.png"));
    mCollision = new CollisionComponent(this);
    mCollision->SetSize(10, 10);
    mMovement = new MoveComponent(this);
    mMovement->SetForwardSpeed(500.0f);
    mTimer = SDL_GetTicks()/1000.0f;
}

void Bullet::UpdateActor(float deltaTime){
    float curr = SDL_GetTicks()/1000.0f;
    if (curr - mTimer > 2.0f){
        this->SetState(Actor::EDead);
    }
    for (Plane* p : GetGame()->mPlanes){
        if (this->GetCollision()->Intersect(p->GetCollision())){
            this->SetState(Actor::EDead);
            p->SetState(Actor::EDead);
        }
    }
}
