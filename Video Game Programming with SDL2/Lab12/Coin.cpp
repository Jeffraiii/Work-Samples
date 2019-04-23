//
//  Coin.cpp
//  Game-mac
//
//  Created by tml on 4/16/18.
//  Copyright © 2018 Sanjay Madhav. All rights reserved.
//

#include <stdio.h>
#include "Game.h"
#include "Coin.h"
#include "Renderer.h"
#include "HUD.h"

Coin::Coin(class Game* game): Actor(game){
    mMesh = new MeshComponent(this);
    mMesh->SetMesh(GetGame()->GetRenderer()->GetMesh("Assets/Coin.gpmesh"));
    mCollision = new CollisionComponent(this);
    mCollision->SetSize(100, 100, 100);
}

void Coin::UpdateActor(float deltaTime){
    mRotation += deltaTime * Math::Pi;
    if (mCollision->Intersect(GetGame()->GetPlayer()->GetCollision())){
        Mix_PlayChannel(-1, GetGame()->GetSound("Assets/Sounds/Coin.wav"), 0);
        GetGame()->GetHUD()->AddCoin();
        SetState(EDead);
    }
}
