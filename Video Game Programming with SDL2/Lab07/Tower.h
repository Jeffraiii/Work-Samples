//
//  Tower.h
//  Game-mac
//
//  Created by tml on 3/7/18.
//  Copyright © 2018 Sanjay Madhav. All rights reserved.
//

#ifndef Tower_h
#define Tower_h
#include "Actor.h"
#include "Plane.h"
#include "Bullet.h"

class Tower: public Actor {
public:
    Tower(class Game* game);
    void UpdateActor(float deltaTime) override;

private:
    float mTimer;
    Plane* findClosest();
};

#endif /* Tower_h */
