#pragma once

// TODO
#include "SDL/SDL.h"
#include <vector>
#include <unordered_map>
#include <string>
#include <algorithm>
#include "SpriteComponent.h"
#include "SDL/SDL_mixer.h"
#include "Background.h"
#include "AnimatedSprite.h"
#include "Math.h"
#include "Grid.h"
#include "Tile.h"
#include "Plane.h"

class Actor;


class Game{
public:
    Game();
    bool Initialize();
    void Shutdown();
    void RunLoop();
    void AddActor(Actor* actor);
    void RemoveActor(Actor* actor);
    void AddSprite(SpriteComponent* sc);
    void RemoveSprite(SpriteComponent* sc);
    SDL_Texture* GetTexture(const char* filename);
    Mix_Chunk* GetSound(const std::string& filename);
    const Vector2& GetCameraPos() const {return mCameraPos;};
    void SetCameraPos(const Vector2& cameraPos);
//    void LoadNextLevel();
//    int numLevels;
    Grid* GetGrid(){return mGrid;}
    std::vector<Plane*> mPlanes;

    
private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    int WALL_THICKNESS;
    int PADDLE_HEIGHT;
    bool quit;
    std::vector<Actor*> actors;
    SDL_Point speed;
    Uint32 prevTime;
    int dir;
    SDL_Texture* texture;
    std::unordered_map<std::string, SDL_Texture*> hashmap;
    void ProcessInput();
    void UpdateGame();
    void GenerateOutput();
    void LoadData();
    void UnloadData();
    void LoadTexture(const char* filename);
    std::vector<SpriteComponent*> mSprites;
    void LoadSound(const std::string& filename);
    std::unordered_map<std::string, Mix_Chunk*> sounds;
    Vector2 mCameraPos;
//    int levelIndex;
    Grid* mGrid;
};
