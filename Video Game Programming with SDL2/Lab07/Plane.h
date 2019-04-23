//
//  Plane.h
//  Game-mac
//
//  Created by tml on 3/7/18.
//  Copyright © 2018 Sanjay Madhav. All rights reserved.
//

#ifndef Plane_h
#define Plane_h
#include "Actor.h"
#include "PlaneMove.h"

class Plane: public Actor{
public:
    Plane(class Game* game);
    ~Plane();
};

#endif /* Plane_h */
