#include "Actor.h"

class Ship: public Actor {
    
public:
    Ship(class Game* game):Actor(game){
        mSprite = new SpriteComponent(this);
        mMovement = new MoveComponent(this);
    }
    void ActorInput(const Uint8* keyState) override{
        if (keyState[SDL_SCANCODE_UP]){
            mSprite->SetTexture(mGame->GetTexture("Assets/ShipThrust.png"));
            mMovement->SetForwardSpeed(FORWARD_SPEED);
        }
        else if (keyState[SDL_SCANCODE_DOWN]){
            mSprite->SetTexture(mGame->GetTexture("Assets/ShipThrust.png"));
            mMovement->SetForwardSpeed(-FORWARD_SPEED);
        }
        else{
            mSprite->SetTexture(mGame->GetTexture("Assets/Ship.png"));
            mMovement->SetForwardSpeed(0);
        }
        if (keyState[SDL_SCANCODE_LEFT]){
            mMovement->SetAngularSpeed(ANGULAR_SPEED);
        }
        else if (keyState[SDL_SCANCODE_RIGHT]){
            mMovement->SetAngularSpeed(-ANGULAR_SPEED);
        }
        else{
            mMovement->SetAngularSpeed(0);
        }
    }

private:
    float FORWARD_SPEED = 50;
    float ANGULAR_SPEED = 1;
};
