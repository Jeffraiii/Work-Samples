//
//  BallMove.h
//  Game-mac
//
//  Created by tml on 2/6/18.
//  Copyright © 2018 Sanjay Madhav. All rights reserved.
//

#ifndef BallMove_h
#define BallMove_h
#include "MoveComponent.h"
#include "Math.h"

class BallMove: public MoveComponent{
public:
    BallMove(class Actor* owner);
    void Update(float deltaTime) override;
private:
    Vector2 mVelocity;
};

#endif /* BallMove_h */
