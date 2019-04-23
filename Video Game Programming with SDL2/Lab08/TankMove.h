//
//  TankMove.h
//  Game-mac
//
//  Created by tml on 3/14/18.
//  Copyright © 2018 Sanjay Madhav. All rights reserved.
//

#ifndef TankMove_h
#define TankMove_h
#include "MoveComponent.h"
#include "SDL/SDL.h"


class TankMove: public MoveComponent {
public:
    TankMove(class Actor* owner);
    void Update(float deltaTime) override;
    void ProcessInput(const Uint8* keyState) override;
    void SetPlayerTwo();
private:
    SDL_Scancode mForwardKey;
    SDL_Scancode mBackKey;
    SDL_Scancode mLeftKey;
    SDL_Scancode mRightKey;
    
};

#endif /* TankMove_h */
