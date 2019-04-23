#pragma once

// TODO
#include "SDL/SDL.h"
#include "SDL/SDL_ttf.h"

class Game{
public:
    Game();
    bool Initialize();
    void Shutdown();
    void RunLoop();
private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    int WALL_THICKNESS;
    int PADDLE_HEIGHT;
    bool quit;
    SDL_Point paddle, ball, speed;
    Uint32 prevTime;
    int dir;
    void ProcessInput();
    void UpdateGame();
    void GenerateOutput();
};
