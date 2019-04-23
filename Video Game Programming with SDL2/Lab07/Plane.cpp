//
//  Plane.cpp
//  Game-mac
//
//  Created by tml on 3/7/18.
//  Copyright © 2018 Sanjay Madhav. All rights reserved.
//

#include <stdio.h>
#include "Plane.h"
#include "Game.h"

Plane::Plane(class Game* game):Actor(game){
    this->SetPosition(this->GetGame()->GetGrid()->GetStartTile()->GetPosition());
    mSprite = new SpriteComponent(this, 200);
    mSprite->SetTexture(GetGame()->GetTexture("Assets/Airplane.png"));
    mCollision = new CollisionComponent(this);
    mCollision->SetSize(64, 64);
    mMovement = new PlaneMove(this);
    GetGame()->mPlanes.push_back(this);
}

Plane::~Plane(){
    GetGame()->mPlanes.erase(std::find(GetGame()->mPlanes.begin(), GetGame()->mPlanes.end(), this));
}
