//
//  Tower.cpp
//  Game-mac
//
//  Created by tml on 3/7/18.
//  Copyright © 2018 Sanjay Madhav. All rights reserved.
//

#include <stdio.h>
#include "Tower.h"
#include "Game.h"

Tower::Tower(class Game* game):Actor(game){
    mSprite = new SpriteComponent(this, 200);
    mSprite->SetTexture(this->GetGame()->GetTexture("Assets/Tower.png"));
    mTimer = SDL_GetTicks()/1000.0f;
}

void Tower::UpdateActor(float deltaTime){
    float curr = SDL_GetTicks()/1000.0f;
    if (curr - mTimer >= 2){
        Plane* closest = this->findClosest();
        Vector2 v = closest->GetPosition() - this->GetPosition();
        if (v.Length() < 100.0f){
            this->SetRotation(atan2(-v.y, v.x));
            Bullet* b = new Bullet(GetGame());
            b->SetRotation(this->GetRotation());
            b->SetPosition(this->GetPosition());
        }
        mTimer = curr;
    }
}

Plane* Tower::findClosest(){
    Plane* min = GetGame()->mPlanes[0];
    for (Plane* p : GetGame()->mPlanes){
        if ((p->GetPosition() - this->GetPosition()).Length() < (min->GetPosition() - this->GetPosition()).Length()){
            min = p;
        }
    }
    return min;
}

