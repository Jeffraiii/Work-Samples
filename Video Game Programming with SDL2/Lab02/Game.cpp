//
//  Game.cpp
//  Game-mac
//
//  Created by Sanjay Madhav on 5/31/17.
//  Copyright © 2017 Sanjay Madhav. All rights reserved.
//

#include "Game.h"

// TODO
Game::Game(){
    quit = true;
    WALL_THICKNESS = 15;
    PADDLE_HEIGHT = 100;
}

bool Game::Initialize(){
    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO)==0){
        window = SDL_CreateWindow("Pong", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1024, 768, 0);
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC);
        TTF_Init();
        paddle.x = 5;
        paddle.y = 364;
        ball.x = 510;
        ball.y = 382;
        speed.x = 80;
        speed.y = 80;
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
    if (state[SDL_SCANCODE_ESCAPE]){
        quit = false;
    }
    if (state[SDL_SCANCODE_UP]){
        dir = -1;
    }
    if (state[SDL_SCANCODE_DOWN]){
        dir = 1;
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
    if (dir == 1){
        paddle.y += dir * 50 * deltaTime + 1;
    }
    else if (dir == -1){
        paddle.y += dir * 50 * deltaTime;
    }
    if (paddle.y < WALL_THICKNESS){
        paddle.y = WALL_THICKNESS;
    }
    else if (paddle.y > 768-PADDLE_HEIGHT-WALL_THICKNESS){
        paddle.y = 768-PADDLE_HEIGHT-WALL_THICKNESS;
    }
    ball.x += speed.x * deltaTime;
    ball.y += speed.y * deltaTime;
    if (ball.y <= WALL_THICKNESS){
        ball.y = WALL_THICKNESS;
        speed.y *= -1;
    }
    else if (ball.y >= 768 - WALL_THICKNESS - WALL_THICKNESS){
        ball.y = 768 - WALL_THICKNESS - WALL_THICKNESS;
        speed.y *= -1;
    }
    else if (ball.x >= 1024 - WALL_THICKNESS - WALL_THICKNESS){
        ball.x = 1024 - WALL_THICKNESS - WALL_THICKNESS;
        speed.x *= -1;
    }
    else if ((ball.x <= 5 + WALL_THICKNESS && ball.x >= 0) && (ball.y >= paddle.y - WALL_THICKNESS && ball.y < paddle.y + PADDLE_HEIGHT)){
        ball.x = 5 + WALL_THICKNESS;
        speed.x *= -1;
    }
    else if (ball.x <=0){
        quit = false;
    }
    dir = 0;
}

void Game::GenerateOutput(){
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_Rect topWall, rightWall, bottomWall, paddleBlcok;
    SDL_Rect ballBlock = {ball.x,ball.y,WALL_THICKNESS,WALL_THICKNESS};
    topWall.x = 0;
    topWall.y = 0;
    topWall.w = 1024;
    topWall.h = WALL_THICKNESS;
    rightWall.x = 1024-WALL_THICKNESS;
    rightWall.y = 0;
    rightWall.w = WALL_THICKNESS;
    rightWall.h = 768;
    bottomWall.x = 0;
    bottomWall.y = 768-WALL_THICKNESS;
    bottomWall.w = 1024;
    bottomWall.h = WALL_THICKNESS;
    paddleBlcok.x = paddle.x;
    paddleBlcok.y = paddle.y;
    paddleBlcok.w = WALL_THICKNESS;
    paddleBlcok.h = PADDLE_HEIGHT;
    SDL_RenderFillRect(renderer, &topWall);
    SDL_RenderFillRect(renderer, &rightWall);
    SDL_RenderFillRect(renderer, &bottomWall);
    SDL_RenderFillRect(renderer, &paddleBlcok);
    SDL_RenderFillRect(renderer, &ballBlock);
    
    TTF_Font* Sans = TTF_OpenFont("ARIAL.TTF", 12); //this opens a font style and sets a size
    
    SDL_Color White = {255, 255, 255};  // this is the color in rgb format, maxing out all would give you the color white, and it will be your text's color
    
    SDL_Surface* surfaceMessage = TTF_RenderText_Solid(Sans, "put your text here", White); // as TTF_RenderText_Solid could only be used on SDL_Surface then you have to create the surface first
    
    SDL_Texture* Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage); //now you can convert it into a texture
    
    SDL_Rect Message_rect; //create a rect
    Message_rect.x = 0;  //controls the rect's x coordinate
    Message_rect.y = 0; // controls the rect's y coordinte
    Message_rect.w = 100; // controls the width of the rect
    Message_rect.h = 50; // controls the height of the rect
    
    //Mind you that (0,0) is on the top left of the window/screen, think a rect as the text's box, that way it would be very simple to understance
    
    //Now since it's a texture, you have to put RenderCopy in your game loop area, the area where the whole code executes
    
    SDL_RenderCopy(renderer, Message, NULL, &Message_rect); //you put the renderer's name first, the Message, the crop size(you can ignore this if you don't want to dabble with cropping), and the rect which is the size and coordinate of your texture

    
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
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
