//
//  Checkpoint.h
//  Game-mac
//
//  Created by tml on 4/12/18.
//  Copyright © 2018 Sanjay Madhav. All rights reserved.
//

#ifndef Checkpoint_h
#define Checkpoint_h
#include "Actor.h"
#include <string>

class Checkpoint: public Actor {
    
public:
    Checkpoint(class Game* game);
    void UpdateActor(float deltaTime) override;
    void SetLevelString(std::string str) {mLevelString = str;}
private:
    bool isActive;
    std::string mLevelString;
};

#endif /* Checkpoint_h */
