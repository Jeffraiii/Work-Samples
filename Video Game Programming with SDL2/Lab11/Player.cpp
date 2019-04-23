//
//  Player.cpp
//  Game-mac
//
//  Created by tml on 3/22/18.
//  Copyright © 2018 Sanjay Madhav. All rights reserved.
//

#include <stdio.h>
#include "Player.h"
#include "PlayerMove.h"

Player::Player(class Game* game):Actor(game){
    mMovement = new PlayerMove(this);
    mCollision = new  CollisionComponent(this);
    mCollision->SetSize(50.0f, 175.0f, 50.0f);
    mCamera = new CameraComponent(this);
}
