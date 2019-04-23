//
//  Paddle.cpp
//  Game-mac
//
//  Created by tml on 2/6/18.
//  Copyright © 2018 Sanjay Madhav. All rights reserved.
//

#include <stdio.h>
#include "Game.h"
#include "Paddle.h"

Paddle::Paddle(class Game* game):Actor(game){
    mSprite = new SpriteComponent(this);
    mMovement = new PaddleMove(this);
    mCollision = new CollisionComponent(this);
    mCollision->SetSize(104, 24);
}

void Paddle::ActorInput(const Uint8* keyState){
    if (keyState[SDL_SCANCODE_LEFT]){
        mMovement->SetForwardSpeed(-SPEED);
    }
    else if (keyState[SDL_SCANCODE_RIGHT]){
        mMovement->SetForwardSpeed(SPEED);
    }
    else{
        mMovement->SetForwardSpeed(0);
    }
}
