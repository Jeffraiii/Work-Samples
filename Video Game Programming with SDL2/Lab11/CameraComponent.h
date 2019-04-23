//
//  CameraComponent.h
//  Game-mac
//
//  Created by tml on 3/22/18.
//  Copyright © 2018 Sanjay Madhav. All rights reserved.
//

#ifndef CameraComponent_h
#define CameraComponent_h
#include "Component.h"
#include "Math.h"

class CameraComponent: public Component {
public:
    CameraComponent(class Actor* owner);
    void Update(float deltaTime) override;
    float GetPitchSpeed() {return mPitchSpeed;}
    void SetPitchSpeed(float s) {mPitchSpeed = s;}
private:
    float mPitchAngle;
    float mPitchSpeed;
};

#endif /* CameraComponent_h */
