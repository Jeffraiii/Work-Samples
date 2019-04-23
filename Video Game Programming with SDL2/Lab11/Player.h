//
//  Player.h
//  Game-mac
//
//  Created by tml on 3/22/18.
//  Copyright © 2018 Sanjay Madhav. All rights reserved.
//

#ifndef Player_h
#define Player_h
#include "Actor.h"

class Player: public Actor {
public:
    Player(class Game* game);
    void SetRespawnPos(Vector3 pos) {mRespawnPos = pos;}
    Vector3 GetRespawnPos() {return mRespawnPos;}
private:
    Vector3 mRespawnPos;
};

#endif /* Player_h */
