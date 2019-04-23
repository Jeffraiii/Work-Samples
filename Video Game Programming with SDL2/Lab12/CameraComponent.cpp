//
//  CameraComponent.cpp
//  Game-mac
//
//  Created by tml on 3/22/18.
//  Copyright © 2018 Sanjay Madhav. All rights reserved.
//

#include <stdio.h>
#include "CameraComponent.h"
#include "Actor.h"
#include "Game.h"
#include "Renderer.h"

CameraComponent::CameraComponent(class Actor* owner):Component(owner){
    
}

void CameraComponent::Update(float deltaTime){
    mPitchAngle += deltaTime * mPitchSpeed;
    if (mPitchAngle < -Math::PiOver2/2){
        mPitchAngle = -Math::PiOver2/2;
    }
    else if (mPitchAngle > Math::PiOver2/2){
        mPitchAngle = Math::PiOver2/2;
    }
    Matrix4 yaw = Matrix4::CreateRotationZ(mOwner->GetRotation());
    Matrix4 pitch = Matrix4::CreateRotationY(mPitchAngle);
    Matrix4 combined = pitch * yaw;
    Vector3 trans = Vector3::Transform(Vector3(1,0,0), combined);
    mOwner->GetGame()->GetRenderer()->SetViewMatrix(Matrix4::CreateLookAt(mOwner->GetPosition(), mOwner->GetPosition() + trans, Vector3(0,0,1)));
    
}
