//
//  Ball.cpp
//  Game-mac
//
//  Created by tml on 2/6/18.
//  Copyright © 2018 Sanjay Madhav. All rights reserved.
//

#include <stdio.h>
#include "Ball.h"
#include "Game.h"

Ball::Ball(class Game* game):Actor(game){
    mSprite = new SpriteComponent(this);
    mMovement = new BallMove(this);
    mCollision = new CollisionComponent(this);
    mCollision->SetSize(10, 10);
}
