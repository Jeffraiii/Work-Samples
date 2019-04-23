//
//  Game.cpp
//  Game-mac
//
//  Created by Sanjay Madhav on 5/31/17.
//  Copyright © 2017 Sanjay Madhav. All rights reserved.
//

#include "Game.h"
#include "Actor.h"
#include "SDL/SDL_image.h"
#include <fstream>

// TODO
Game::Game(){
    quit = true;
    WALL_THICKNESS = 15;
    PADDLE_HEIGHT = 100;
    blocks = new std::vector<Block*>;
}

Paddle* Game::GetPaddle(){
    return mPaddle;
}

void Game::AddActor(Actor* actor){
    actors.push_back(actor);
}

void Game::RemoveActor(Actor* actor){
    std::vector<Actor*>::iterator it = std::find(actors.begin(), actors.end(),actor);
    actors.erase(it);
}

void Game::AddSprite(SpriteComponent* sc){
    mSprites.push_back(sc);
    std::sort(mSprites.begin(), mSprites.end(), [](SpriteComponent* a, SpriteComponent* b){return a->GetDrawOrder()<b->GetDrawOrder();});
}

void Game::RemoveSprite(SpriteComponent* sc){
    std::vector<SpriteComponent*>::iterator it = std::find(mSprites.begin(), mSprites.end(), sc);
    mSprites.erase(it);
}

void Game::LoadTexture(const char* filename){
    SDL_Surface* surface = IMG_Load(filename);
    std::string fn(filename);
    hashmap[fn] = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
}

SDL_Texture* Game::GetTexture(const char* filename){
    std::string fn(filename);
    return hashmap.find(fn)->second;
}

void Game::LoadData(){
    LoadTexture("Assets/Background.png");
    LoadTexture("Assets/Ball.png");
    LoadTexture("Assets/BlockA.png");
    LoadTexture("Assets/BlockB.png");
    LoadTexture("Assets/BlockC.png");
    LoadTexture("Assets/BlockD.png");
    LoadTexture("Assets/BlockE.png");
    LoadTexture("Assets/BlockF.png");
    LoadTexture("Assets/Paddle.png");
    
    Actor* background = new Actor(this);
    SpriteComponent* sc1 = new SpriteComponent(background);
    sc1->SetTexture(GetTexture("Assets/Background.png"));
    background->SetSprite(sc1);
    background->SetPosition(Vector2(512, 384));
    
    std::ifstream ifile("Assets/level.txt");
    int x = 64;
    int y = 48;
    std::string curr;
    while (ifile>>curr){
        for (char a : curr){
            if (a!='.'){
                Block* b = new Block(this);
                b->SetTexture(a);
                b->SetPosition(Vector2(x, y));
            }
            x+=64;
        }
        x=64;
        y+=32;
    }
    
    mPaddle = new Paddle(this);
    mPaddle->GetSprite()->SetTexture(GetTexture("Assets/Paddle.png"));
    mPaddle->SetPosition(Vector2(512, 748));
    
    Ball* ball = new Ball(this);
    ball->GetSprite()->SetTexture(GetTexture("Assets/Ball.png"));
    ball->SetPosition(Vector2(512, 648));
}

void Game::UnloadData(){
    while (!actors.empty()){
        actors.pop_back();
    }
    for (std::unordered_map<std::string, SDL_Texture*>::iterator it = hashmap.begin();it!=hashmap.end();it++){
        SDL_DestroyTexture(it->second);
    }
}

bool Game::Initialize(){
    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO)==0){
        window = SDL_CreateWindow("Lab04", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1024, 768, 0);
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC);
        IMG_Init(IMG_INIT_PNG);
        speed.x = 80;
        speed.y = 80;
        LoadData();
        prevTime = SDL_GetTicks();
        return true;
    }
    return false;
}

void Game::ProcessInput(){
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT){
            quit = false;
        }
    }
    const Uint8 *state = SDL_GetKeyboardState(NULL);
    for (int i=0; i<actors.size(); ++i){
        actors[i]->ProcessInput(state);
    }
    if (state[SDL_SCANCODE_ESCAPE]){
        quit = false;
    }
}

void Game::UpdateGame(){
    Uint32 currTime = SDL_GetTicks();
    while ((float)(currTime-prevTime) < 0.016f) {
        currTime = SDL_GetTicks();
    }
    float deltaTime = currTime - prevTime;
    if (deltaTime > 0.05f){
        deltaTime = 0.05f;
    }
    prevTime = currTime;
    std::vector<Actor*> temp = actors;
    for (Actor* a : temp){
        a->Update(deltaTime);
    }
    std::vector<Actor*> dead;
    for (Actor* a : actors){
        if (a->GetState()==a->EDead){
            dead.push_back(a);
        }
    }
    for (Actor* a : dead){
        delete a;
    }
}

void Game::GenerateOutput(){
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
    SDL_RenderClear(renderer);
    for (unsigned int i = 0; i < mSprites.size(); i++){
        mSprites[i]->Draw(renderer);
    }
    SDL_RenderPresent(renderer);
}

void Game::RunLoop(){
    while (quit){
        ProcessInput();
        UpdateGame();
        GenerateOutput();
    }
}

void Game::Shutdown(){
    UnloadData();
    IMG_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
