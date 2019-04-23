//
//  Block.cpp
//  Game-mac
//
//  Created by tml on 3/8/18.
//  Copyright © 2018 Sanjay Madhav. All rights reserved.
//

#include <stdio.h>
#include "Game.h"
#include "Block.h"
#include "Renderer.h"

Block::Block(class Game* game):Actor(game){
    mMesh = new MeshComponent(this);
    mMesh->SetMesh(game->GetRenderer()->GetMesh("Assets/Cube.gpmesh"));
    mScale = 64.0f;
    mCollision = new CollisionComponent(this);
    mCollision->SetSize(1.0f, 1.0f, 1.0f);
    this->GetGame()->GetBlocks().push_back(this);
}

Block::~Block(){
    this->GetGame()->GetBlocks().erase(std::find(this->GetGame()->GetBlocks().begin(), this->GetGame()->GetBlocks().end(), this));
}
