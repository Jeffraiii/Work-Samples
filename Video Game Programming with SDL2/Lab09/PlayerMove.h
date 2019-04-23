//
//  PlayerMove.h
//  Game-mac
//
//  Created by tml on 3/22/18.
//  Copyright © 2018 Sanjay Madhav. All rights reserved.
//

#ifndef PlayerMove_h
#define PlayerMove_h
#include "MoveComponent.h"

class PlayerMove: public MoveComponent {
public:
    enum MoveState{
        OnGround,
        Jump,
        Falling
    };
    enum CollSide{
        None, Top, Bottom, Side
    };
    PlayerMove(class Actor* owner);
    void Update(float deltaTime) override;
    void ProcessInput(const Uint8* keyState) override;
    void ChangeState(MoveState state);
protected:
    void UpdateOnGround(float deltaTime);
    void UpdateJump(float deltaTime);
    void UpdateFalling(float deltaTime);
    CollSide FixCollision(class CollisionComponent* self, class CollisionComponent* block);
private:
    MoveState mCurrentState;
    float mZSpeed;
    const float Gravity = -980.0f;
    const float JumpSpeed = 500.0f;
};

#endif /* PlayerMove_h */
