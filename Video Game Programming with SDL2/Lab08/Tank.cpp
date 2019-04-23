//
//  Tank.cpp
//  Game-mac
//
//  Created by tml on 3/9/18.
//  Copyright © 2018 Sanjay Madhav. All rights reserved.
//

#include <stdio.h>
#include "Tank.h"
#include "Game.h"
#include "Renderer.h"
#include "TankMove.h"
#include "Bullet.h"

Tank::Tank(class Game* game):Actor(game){
    mMesh = new MeshComponent(this);
    mMesh->SetMesh(game->GetRenderer()->GetMesh("Assets/TankBase.gpmesh"));
    mMovement = new TankMove(this);
    mTurret = new Turret(game);
    mCollision = new CollisionComponent(this);
    mCollision->SetSize(30.0f, 30.0f, 30.0f);
    mFiringKey = SDL_SCANCODE_LSHIFT;
    fired = false;
}

void Tank::UpdateActor(float deltaTime){
    mTurret->SetPosition(this->GetPosition());
}

void Tank::SetPlayerTwo(){
    ((TankMove*) mMovement)->SetPlayerTwo();
    mTurret->SetPlayerTwo();
    mMesh->SetTextureIndex(1);
    mFiringKey = SDL_SCANCODE_RSHIFT;
}

void Tank::Fire(){
    Bullet* b = new Bullet(GetGame());
    b->SetPosition(mPosition);
    b->SetRotation(mTurret->GetRotation());
    b->SetShooter(this);
}

void Tank::Respawn(){
    this->SetPosition(originalPos);
    this->SetRotation(0);
    mTurret->SetRotation(0);
}

void Tank::ActorInput(const Uint8 *keyState){
    if (keyState[mFiringKey]){
        if (!fired){
            Fire();
            fired = true;
        }
    }
    else{
        fired = false;
    }
}
