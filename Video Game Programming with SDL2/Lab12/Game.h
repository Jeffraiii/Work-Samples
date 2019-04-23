#pragma once
#include "SDL/SDL.h"
#include "SDL/SDL_mixer.h"
#include <unordered_map>
#include <string>
#include <vector>
#include "Math.h"
#include "Block.h"
#include <queue>
#include "Player.h"
#include "Checkpoint.h"
#include "Arrow.h"
#include "Coin.h"
#include "SDL/SDL_ttf.h"
#include "HUD.h"

class Game
{
public:
	Game();
	bool Initialize();
	void RunLoop();
	void Shutdown();

	void AddActor(class Actor* actor);
	void RemoveActor(class Actor* actor);

	void LoadSound(const std::string& fileName);
	Mix_Chunk* GetSound(const std::string& fileName);

	void LoadLevel(const std::string& fileName);

	class Renderer* GetRenderer() {	return mRenderer; }
    
    std::vector<Block*> & GetBlocks(){return mBlocks;}
    
    void AddCheckpoint(Checkpoint* cp){mCheckpoints.push(cp);}
    
    std::queue<Checkpoint*> & GetCheckpoint(){return mCheckpoints;}
    
    void SetPlayer(Player* p){mPlayer=p;}
    
    Player* GetPlayer() {return mPlayer;}
    
    void SetNextLevel(std::string s) {mNextLevel = s;}
    
    void LoadNextLevel();
    
    HUD* GetHUD() {return mHUD;}
    
private:
	void ProcessInput();
	void UpdateGame();
	void GenerateOutput();
	void LoadData();
	void UnloadData();

	// Map of textures loaded
	std::unordered_map<std::string, SDL_Texture*> mTextures;
	std::unordered_map<std::string, Mix_Chunk*> mSounds;

	// All the actors in the game
	std::vector<class Actor*> mActors;

	class Renderer* mRenderer;

	Uint32 mTicksCount;
	bool mIsRunning;
    std::vector<Block*> mBlocks;
    std::queue<Checkpoint*> mCheckpoints;
    Player* mPlayer;
    std::string mNextLevel;
    Arrow* mArrow;
    HUD* mHUD;
};
