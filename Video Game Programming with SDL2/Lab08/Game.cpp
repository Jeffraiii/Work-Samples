//
//  Game.cpp
//  Game-mac
//
//  Created by Sanjay Madhav on 5/31/17.
//  Copyright © 2017 Sanjay Madhav. All rights reserved.
//

#include "Game.h"
#include <algorithm>
#include "Actor.h"
#include <fstream>
#include "Renderer.h"

Game::Game()
:mIsRunning(true)
{
	
}

bool Game::Initialize()
{
	if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO) != 0)
	{
		SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
		return false;
	}

	// TODO: Create renderer
    mRenderer = new Renderer(this);
    if (mRenderer->Initialize(1024.0f, 768.0f) == false){
        return false;
    }

	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

	LoadData();

	mTicksCount = SDL_GetTicks();
	
	return true;
}

void Game::RunLoop()
{
	while (mIsRunning)
	{
		ProcessInput();
		UpdateGame();
		GenerateOutput();
	}
}

void Game::ProcessInput()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
			case SDL_QUIT:
				mIsRunning = false;
				break;
		}
	}
	
	const Uint8 *state = SDL_GetKeyboardState(NULL);
	if (state[SDL_SCANCODE_ESCAPE])
	{
		mIsRunning = false;
	}

	for (auto actor : mActors)
	{
		actor->ProcessInput(state);
	}
}

void Game::UpdateGame()
{
	// Compute delta time
	// Wait until 16ms has elapsed since last frame
	while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + 16))
		;

	float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;
	if (deltaTime > 0.05f)
	{
		deltaTime = 0.05f;
	}
	mTicksCount = SDL_GetTicks();
	
	// Make copy of actor vector
	// (iterate over this in case new actors are created)
	std::vector<Actor*> copy = mActors;
	// Update all actors
	for (auto actor : copy)
	{
		actor->Update(deltaTime);
	}

	// Add any dead actors to a temp vector
	std::vector<Actor*> deadActors;
	for (auto actor : mActors)
	{
		if (actor->GetState() == Actor::EDead)
		{
			deadActors.emplace_back(actor);
		}
	}

	// Delete any of the dead actors (which will
	// remove them from mActors)
	for (auto actor : deadActors)
	{
		delete actor;
	}
}

void Game::GenerateOutput()
{
	// TODO: tell renderer to draw
    mRenderer->Draw();
}

void Game::LoadData()
{
	LoadLevel("Assets/Level.txt");
}

void Game::LoadLevel(const std::string& fileName)
{
	std::ifstream file(fileName);
	if (!file.is_open())
	{
		SDL_Log("Failed to load level: %s", fileName.c_str());
	}

	const float topLeftX = -512.0f + 32.0f;
	const float topLeftY = -384.0f + 32.0f;
	size_t row = 0;
	std::string line;
	while (!file.eof())
	{
		std::getline(file, line);
		for (size_t col = 0; col < line.size(); col++)
		{
			// Calculate position at this row/column
			Vector3 pos;
			pos.x = topLeftX + 64.0f * col;
			pos.y = topLeftY + 64.0f * row;

			if (line[col] == 'B')
			{
				// TODO: Create block
                Block * b = new Block(this);
                b->SetPosition(pos);
			}
			else if (line[col] == 'P')
			{
				// TODO: Create player 1
                mTank1 = new Tank(this);
                mTank1->SetPosition(pos);
                mTank1->originalPos = pos;
			}
			else if (line[col] == 'Q')
			{
				// TODO: Create player 2
                mTank2 = new Tank(this);
                mTank2->SetPosition(pos);
                mTank2->SetPlayerTwo();
                mTank2->originalPos = pos;
			}
		}
		row++;
	}
}

void Game::UnloadData()
{
	// Delete actors
	// Because ~Actor calls RemoveActor, have to use a different style loop
	while (!mActors.empty())
	{
		delete mActors.back();
	}

	// Destroy textures
	for (auto i : mTextures)
	{
		SDL_DestroyTexture(i.second);
	}
	mTextures.clear();

	// Destroy sounds
	for (auto s : mSounds)
	{
		Mix_FreeChunk(s.second);
	}
	mSounds.clear();
}

void Game::LoadSound(const std::string& fileName)
{
	Mix_Chunk* chunk = Mix_LoadWAV(fileName.c_str());
	if (!chunk)
	{
		SDL_Log("Failed to load sound file %s", fileName.c_str());
		return;
	}

	mSounds.emplace(fileName, chunk);
}

Mix_Chunk* Game::GetSound(const std::string& fileName)
{
	Mix_Chunk* chunk = nullptr;
	auto iter = mSounds.find(fileName);
	if (iter != mSounds.end())
	{
		chunk = iter->second;
	}
	return chunk;
}

void Game::Shutdown()
{
	UnloadData();
	Mix_CloseAudio();
	// TODO: Delete renderer
    mRenderer->Shutdown();
    delete mRenderer;
	SDL_Quit();
}

void Game::AddActor(Actor* actor)
{
	mActors.emplace_back(actor);
}

void Game::RemoveActor(Actor* actor)
{
	auto iter = std::find(mActors.begin(), mActors.end(), actor);
	if (iter != mActors.end())
	{
		// Swap to end of vector and pop off (avoid erase copies)
		auto iter2 = mActors.end() - 1;
		std::iter_swap(iter, iter2);
		mActors.pop_back();
	}
}
