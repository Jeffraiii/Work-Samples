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
#include "Math.h"

class PlayerMove: public MoveComponent {
public:
    enum MoveState{
        OnGround,
        Jump,
        Falling,
        WallClimb,
        WallRun
    };
    enum CollSide{
        None, Top, Bottom, SideX1, SideX2, SideY1, SideY2
    };
    PlayerMove(class Actor* owner);
    ~PlayerMove();
    void Update(float deltaTime) override;
    void ProcessInput(const Uint8* keyState) override;
    void ChangeState(MoveState state);
    void PhysicsUpdate(float deltaTime);
    void AddForce(const Vector3& force) {mPendingForces += force;}
protected:
    void UpdateOnGround(float deltaTime);
    void UpdateJump(float deltaTime);
    void UpdateFalling(float deltaTime);
    void UpdateWallClimb(float deltaTime);
    void UpdateWallRun(float deltaTime);
    CollSide FixCollision(class CollisionComponent* self, class CollisionComponent* block);
    void FixXYVelocity();
    bool CanWallClimb(CollSide cs);
    bool CanWallRun(CollSide cs);
private:
    MoveState mCurrentState;
//    float mZSpeed;
//    const float Gravity = -980.0f;
//    const float JumpSpeed = 500.0f;
    Vector3 mVelocity;
    Vector3 mAcceleration;
    Vector3 mPendingForces;
    float mMass;
    Vector3 mGravity;
    Vector3 mJumpForce;
    bool mSpacePressed;
    Vector3 mClimbForce;
    float mWallClimbTimer;
    Vector3 mWallRunForce;
    float mWallRunTimer;
    int mRunningSFX;
};

#endif /* PlayerMove_h */
