#include "HUD.h"
#include "Texture.h"
#include "Shader.h"
#include "Game.h"
#include "Renderer.h"
#include "Font.h"
#include <sstream>
#include <iomanip>
#include <math.h>

HUD::HUD(Game* game)
	:mGame(game)
	,mFont(nullptr)
{
	// Load font
	mFont = new Font();
	mFont->Load("Assets/Inconsolata-Regular.ttf");
    mTimerText = mFont->RenderText("00:00.00");
    mCoinText = mFont->RenderText("0/55");
    mCheckpointText = mFont->RenderText("Welcome!");
}

HUD::~HUD()
{
	// Get rid of font
	if (mFont)
	{
		mFont->Unload();
		delete mFont;
	}
}

void HUD::Update(float deltaTime)
{
	// TODO
    mTimer += deltaTime;
    mCheckpointTimer += deltaTime;
    mTimerText->Unload();
    delete mTimerText;
    float temp;
    float fracpart = std::modf(mTimer, &temp);
    int intpart = (int) temp;
    int min = intpart / 60;
    int sec = intpart % 60;
    int frac = fracpart * 100;
    std::stringstream ss;
    ss<<std::setw(2)<<std::setfill('0')<<min;
    ss<<':';
    ss<<std::setw(2)<<std::setfill('0')<<sec;
    ss<<':';
    ss<<std::setw(2)<<std::setfill('0')<<frac;
    std::string text;
    ss >> text;
    mTimerText = mFont->RenderText(text);
}

void HUD::Draw(Shader* shader)
{
	// TODO
    DrawTexture(shader, mTimerText, Vector2(-420.0f, -325.0f));
    DrawTexture(shader, mCoinText, Vector2(-420.0f, -295.0f));
    if (mCheckpointTimer < 5.0f){
        DrawTexture(shader, mCheckpointText, Vector2::Zero);
    }
}

void HUD::DrawTexture(class Shader* shader, class Texture* texture,
				 const Vector2& offset, float scale)
{
	// Scale the quad by the width/height of texture
	Matrix4 scaleMat = Matrix4::CreateScale(
		static_cast<float>(texture->GetWidth()) * scale,
		static_cast<float>(texture->GetHeight()) * scale,
		1.0f);
	// Translate to position on screen
	Matrix4 transMat = Matrix4::CreateTranslation(
		Vector3(offset.x, offset.y, 0.0f));	
	// Set world transform
	Matrix4 world = scaleMat * transMat;
	shader->SetMatrixUniform("uWorldTransform", world);
	// Set current texture
	texture->SetActive();
	// Draw quad
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}

void HUD::AddCoin(){
    numCoins++;
    mCoinText->Unload();
    delete mCoinText;
    std::string text = std::to_string(numCoins) + "/55";
    mCoinText = mFont->RenderText(text);
}

void HUD::UpdateCheckpointText(float deltaTime, std::string s){
    mCheckpointTimer = 0;
    mCheckpointText->Unload();
    delete mCheckpointText;
    std::string text = s;
    mCheckpointText = mFont->RenderText(text);
}
