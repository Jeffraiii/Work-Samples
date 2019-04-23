//
//  Paddle.h
//  Game-mac
//
//  Created by tml on 2/6/18.
//  Copyright © 2018 Sanjay Madhav. All rights reserved.
//

#ifndef Paddle_h
#define Paddle_h
#include "Actor.h"
#include "PaddleMove.h"

class Paddle: public Actor{
public:
    Paddle(class Game* game);
    void ActorInput(const Uint8* keyState) override;
    float SPEED = 200;
};

#endif /* Paddle_h */
