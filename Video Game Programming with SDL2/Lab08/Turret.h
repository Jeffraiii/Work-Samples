//
//  Turret.h
//  Game-mac
//
//  Created by tml on 3/9/18.
//  Copyright © 2018 Sanjay Madhav. All rights reserved.
//

#ifndef Turret_h
#define Turret_h
#include "Actor.h"
#include "SDL/SDL.h"

class Turret: public Actor {
public:
    Turret(class Game* game);
    void ActorInput(const Uint8* keyState) override;
    void SetPlayerTwo();
private:
    SDL_Scancode mLeftKey;
    SDL_Scancode mRightKey;
};

#endif /* Turret_h */
