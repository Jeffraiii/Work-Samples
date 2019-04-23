//
//  PlaneMove.h
//  Game-mac
//
//  Created by tml on 3/7/18.
//  Copyright © 2018 Sanjay Madhav. All rights reserved.
//

#ifndef PlaneMove_h
#define PlaneMove_h
#include "MoveComponent.h"
#include "Tile.h"

class PlaneMove: public MoveComponent{
public:
    PlaneMove(class Actor* owner);
    void Update(float deltaTime) override;
    void SetNextTile(const Tile* n);
private:
    const Tile* mNextTile;
};

#endif /* PlaneMove_h */
