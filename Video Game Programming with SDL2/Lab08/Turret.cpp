//
//  Turret.cpp
//  Game-mac
//
//  Created by tml on 3/9/18.
//  Copyright © 2018 Sanjay Madhav. All rights reserved.
//

#include <stdio.h>
#include "Turret.h"
#include "Game.h"
#include "Renderer.h"

Turret::Turret(class Game* game):Actor(game){
    mMesh = new MeshComponent(this);
    mMesh->SetMesh(game->GetRenderer()->GetMesh("Assets/TankTurret.gpmesh"));
    mMovement = new MoveComponent(this);
    mScale = 1.8f;
    mLeftKey = SDL_SCANCODE_Q;
    mRightKey = SDL_SCANCODE_E;
}

void Turret::ActorInput(const Uint8 *keyState){
    if (keyState[mLeftKey]){
        mMovement->SetAngularSpeed(Math::TwoPi);
    }
    else if (keyState[mRightKey]){
        mMovement->SetAngularSpeed(-Math::TwoPi);
    }
    else{
        mMovement->SetAngularSpeed(0);
    }
}

void Turret::SetPlayerTwo(){
    mLeftKey = SDL_SCANCODE_I;
    mRightKey = SDL_SCANCODE_P;
    mMesh->SetTextureIndex(1);
}
