//
//  Tank.h
//  Game-mac
//
//  Created by tml on 3/9/18.
//  Copyright © 2018 Sanjay Madhav. All rights reserved.
//

#ifndef Tank_h
#define Tank_h
#include "Actor.h"
#include "Turret.h"
#include "SDL/SDL.h"

class Tank: public Actor {
public:
    Tank(class Game* game);
    void UpdateActor(float deltaTime) override;
    void SetPlayerTwo();
    void Fire();
    void Respawn();
    Vector3 originalPos;
    void ActorInput(const Uint8* keyState) override;
private:
    Turret* mTurret;
    SDL_Scancode mFiringKey;
    bool fired;
};

#endif /* Tank_h */
