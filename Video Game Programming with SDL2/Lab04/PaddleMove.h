//
//  PaddleMove.h
//  Game-mac
//
//  Created by tml on 2/6/18.
//  Copyright © 2018 Sanjay Madhav. All rights reserved.
//

#ifndef PaddleMove_h
#define PaddleMove_h
#include "MoveComponent.h"

class PaddleMove: public MoveComponent{
public:
    PaddleMove(class Actor* owner);
    void Update(float deltaTime) override;
};

#endif /* PaddleMove_h */
