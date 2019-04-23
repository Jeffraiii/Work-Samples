#include "Grid.h"
#include "Tile.h"
#include <SDL/SDL.h>
#include <algorithm>
#include "Tower.h"
#include "Plane.h"

Grid::Grid(class Game* game)
	:Actor(game)
	,mSelectedTile(nullptr)
{
	// 7 rows, 16 columns
	mTiles.resize(NumRows);
	for (size_t i = 0; i < mTiles.size(); i++)
	{
		mTiles[i].resize(NumCols);
	}
	
	// Create tiles
	for (size_t i = 0; i < NumRows; i++)
	{
		for (size_t j = 0; j < NumCols; j++)
		{
			mTiles[i][j] = new Tile(GetGame());
			mTiles[i][j]->SetPosition(Vector2(TileSize/2.0f + j * TileSize, StartY + i * TileSize));
		}
	}
	
	// Set start/end tiles
	GetStartTile()->SetTileState(Tile::EStart);
	GetEndTile()->SetTileState(Tile::EBase);
	
	// Set up adjacency lists
	for (size_t i = 0; i < NumRows; i++)
	{
		for (size_t j = 0; j < NumCols; j++)
		{
			if (i > 0)
			{
				mTiles[i][j]->mAdjacent.push_back(mTiles[i-1][j]);
			}
			if (i < NumRows - 1)
			{
				mTiles[i][j]->mAdjacent.push_back(mTiles[i+1][j]);
			}
			if (j > 0)
			{
				mTiles[i][j]->mAdjacent.push_back(mTiles[i][j-1]);
			}
			if (j < NumCols - 1)
			{
				mTiles[i][j]->mAdjacent.push_back(mTiles[i][j+1]);
			}
		}
	}
    TryFindPath();
    UpdatePathTiles();
    mTimer = SDL_GetTicks()/1000.f;
}

void Grid::SelectTile(size_t row, size_t col)
{
	// Make sure it's a valid selection
	Tile::TileState tstate = mTiles[row][col]->GetTileState();
	if (tstate != Tile::EStart && tstate != Tile::EBase)
	{
		// Deselect previous one
		if (mSelectedTile)
		{
			mSelectedTile->ToggleSelect();
		}
		mSelectedTile = mTiles[row][col];
		mSelectedTile->ToggleSelect();
	}
}

Tile* Grid::GetStartTile()
{
	return mTiles[3][0];
}

Tile* Grid::GetEndTile()
{
	return mTiles[3][15];
}

void Grid::ActorInput(const Uint8 * keyState)
{
	// Process mouse click to select a tile
	int x, y;
	Uint32 buttons = SDL_GetMouseState(&x, &y);
	if (SDL_BUTTON(buttons) & SDL_BUTTON_LEFT)
	{
		// Calculate the x/y indices into the grid
		y -= static_cast<int>(StartY - TileSize / 2);
		if (y >= 0)
		{
			x /= static_cast<int>(TileSize);
			y /= static_cast<int>(TileSize);
			if (x >= 0 && x < static_cast<int>(NumCols) && y >= 0 && y < static_cast<int>(NumRows))
			{
				SelectTile(y, x);
			}
		}
	}
    if (keyState[SDL_SCANCODE_SPACE] && mSelectedTile != nullptr){
        mNeedToBuild = true;
    }
}

void Grid::UpdateActor(float deltaTime)
{
    if (mNeedToBuild){
        BuildTower(mSelectedTile);
        mNeedToBuild = false;
    }
    float curr = SDL_GetTicks()/1000.0f;
    if (curr - mTimer >= 1){
        new Plane(GetGame());
        mTimer = curr;
    }
}

bool Grid::TryFindPath(){
    std::vector<Tile*> openSet;
    Tile* curr = this->GetEndTile();
    curr->mInClosedSet = true;
    for (unsigned int i = 0; i < mTiles.size(); i++){
        for (unsigned int j = 0; j < mTiles[i].size(); j++){
            mTiles[i][j]->mInClosedSet = false;
            mTiles[i][j]->g = 0.0f;
        }
    }
    do{
        
        for (Tile* n : curr->mAdjacent){
            if (n->mInClosedSet){
                continue;
            }
            else if (std::find(openSet.begin(), openSet.end(), n) != openSet.end() && !openSet.empty()){
                float new_g = curr->g + 1;
                if (new_g < n->g){
                    n->mParent = curr;
                    n->g = new_g;
                    n->h = (this->GetEndTile()->GetPosition() - n->GetPosition()).Length();
                    n->f = n->g + n->h;
                }
            }
            else{
                n->mParent = curr;
                n->h = (this->GetStartTile()->GetPosition() - n->GetPosition()).Length();
                n->g = curr->g + 1;
                n->f = n->g + n->h;
                if (!n->mBlocked){
                    openSet.push_back(n);
                }
            }
        }
        if (openSet.empty()){
            return false;
        }
        Tile* min = openSet[0];
        for (unsigned int i=0;i<openSet.size();i++){
            
            
                if(openSet[i]->f < min->f){
                    min = openSet[i];
                }
        }
        curr = min;
        
        openSet.erase(std::find(openSet.begin(), openSet.end(), curr));
        curr->mInClosedSet = true;
    }while(curr != this->GetStartTile());
    GetEndTile()->mParent = nullptr;
    return true;
}

void Grid::UpdatePathTiles(){
    for (unsigned int i = 0; i < mTiles.size(); i++){
        for (unsigned int j = 0; j < mTiles[i].size(); j++){
            if (mTiles[i][j] == this->GetEndTile() || mTiles[i][j] == this->GetStartTile()){
                continue;
            }
            mTiles[i][j]->SetTileState(Tile::EDefault);
        }
    }
    Tile* n = this->GetStartTile();
    while (n != this->GetEndTile()){
        if (n == this->GetStartTile() || n == this->GetEndTile()){
            n = n->mParent;
            continue;
        }
        n->SetTileState(Tile::EPath);
        n = n->mParent;
    }
    
}

void Grid::BuildTower(Tile *n){
    if (n->mBlocked){
        return;
    }
    n->mBlocked = true;
    if (!TryFindPath()){
        n->mBlocked = false;
        TryFindPath();
    }
    else{
        Tower* t = new Tower(GetGame());
        t->SetPosition(n->GetPosition());
    }
    UpdatePathTiles();
    
}
