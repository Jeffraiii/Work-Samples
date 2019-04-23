//
//  Arrow.h
//  Game-mac
//
//  Created by tml on 4/12/18.
//  Copyright © 2018 Sanjay Madhav. All rights reserved.
//

#ifndef Arrow_h
#define Arrow_h
#include "Actor.h"

class Arrow: public Actor {
    
public:
    Arrow(class Game* game);
    void UpdateActor(float deltaTime) override;
};

#endif /* Arrow_h */
