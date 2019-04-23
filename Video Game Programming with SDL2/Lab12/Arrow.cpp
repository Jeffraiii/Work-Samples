//
//  Arrow.cpp
//  Game-mac
//
//  Created by tml on 4/12/18.
//  Copyright © 2018 Sanjay Madhav. All rights reserved.
//

#include <stdio.h>
#include "Arrow.h"
#include "Game.h"
#include "Renderer.h"

Arrow::Arrow(class Game* game):Actor(game){
    mMesh = new MeshComponent(this);
    mMesh->SetMesh(GetGame()->GetRenderer()->GetMesh("Assets/Arrow.gpmesh"));
    mScale = 0.15f;
}

void Arrow::UpdateActor(float deltaTime){
    
    if (GetGame()->GetCheckpoint().empty()){
        mQuat = Quaternion::Identity;
    }
    else
    {
        Vector3 pc = GetGame()->GetCheckpoint().front()->GetPosition() - GetGame()->GetPlayer()->GetPosition();
        pc.Normalize();
        float angle = Math::Acos(Vector3::Dot(Vector3(1, 0, 0), pc));
        Vector3 axis = Vector3::Cross(Vector3(1, 0, 0), pc);
        axis.Normalize();
        if (angle == 1){
            mQuat = Quaternion::Identity;
        }
        else
        {
            mQuat = Quaternion(axis, angle);
        }
    }
    mPosition = GetGame()->GetRenderer()->Unproject(Vector3(0.0f, 250.0f, 0.1f));
}
