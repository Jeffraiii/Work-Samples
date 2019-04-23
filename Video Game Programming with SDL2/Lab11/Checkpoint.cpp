//
//  Checkpoint.cpp
//  Game-mac
//
//  Created by tml on 4/12/18.
//  Copyright © 2018 Sanjay Madhav. All rights reserved.
//

#include <stdio.h>
#include "Checkpoint.h"
#include "Game.h"
#include "Renderer.h"
#include <queue>
#include "Player.h"

Checkpoint::Checkpoint(class Game* game):Actor(game){
    mMesh = new MeshComponent(this);
    mMesh->SetMesh(game->GetRenderer()->GetMesh("Assets/Checkpoint.gpmesh"));
    mCollision = new CollisionComponent(this);
    mCollision->SetSize(25, 25, 25);
    mMesh->SetTextureIndex(1);
    isActive = false;
}

void Checkpoint::UpdateActor(float deltaTime){
    if (this == GetGame()->GetCheckpoint().front()){
        isActive = true;
    }
    if (isActive){
        mMesh->SetTextureIndex(0);
        if (mCollision->Intersect(GetGame()->GetPlayer()->GetCollision())){
            Mix_PlayChannel(-1, GetGame()->GetSound("Assets/Sounds/Checkpoint.wav"), 0);
            if (!mLevelString.empty()){
                GetGame()->SetNextLevel(mLevelString);
            }
            GetGame()->GetCheckpoint().pop();
            GetGame()->GetPlayer()->SetRespawnPos(mPosition);
            SetState(EDead);
        }
    }
}
