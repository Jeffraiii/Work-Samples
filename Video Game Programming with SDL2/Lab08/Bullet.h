//
//  Bullet.h
//  Game-mac
//
//  Created by tml on 3/14/18.
//  Copyright © 2018 Sanjay Madhav. All rights reserved.
//

#ifndef Bullet_h
#define Bullet_h
#include "Actor.h"
#include "Tank.h"

class Bullet: public Actor {
public:
    Bullet(class Game* game);
    void SetShooter(Tank* t) {mShooter = t;}
    void UpdateActor(float deltaTime) override;
private:
    Tank* mShooter;
};

#endif /* Bullet_h */
