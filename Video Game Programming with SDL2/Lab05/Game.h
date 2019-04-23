#pragma once

// TODO
#include "SDL/SDL.h"
#include <vector>
#include <unordered_map>
#include <string>
#include <algorithm>
#include "SpriteComponent.h"
#include "Block.h"
#include "Player.h"
#include "Barrel.h"
#include "BarrelSpawner.h"
#include "SDL/SDL_mixer.h"

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
    std::vector<Block*>* GetBlocks() const{return blocks;}
    Player* GetPlayer() const{return mPlayer;}
    Mix_Chunk* GetSound(const std::string& filename);
    Vector2 GetPlayerStart() const{return playerStart;}
    
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
    std::vector<Block*>* blocks;
    Player* mPlayer;
    void LoadSound(const std::string& filename);
    std::unordered_map<std::string, Mix_Chunk*> sounds;
    Vector2 playerStart;
};