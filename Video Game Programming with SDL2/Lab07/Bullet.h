//
//  Bullet.h
//  Game-mac
//
//  Created by tml on 3/7/18.
//  Copyright © 2018 Sanjay Madhav. All rights reserved.
//

#ifndef Bullet_h
#define Bullet_h
#include "Actor.h"
#include "Plane.h"

class Bullet: public Actor {
    
public:
    Bullet(class Game* game);
    void UpdateActor(float deltaTime) override;
private:
    float mTimer;
};

#endif /* Bullet_h */
