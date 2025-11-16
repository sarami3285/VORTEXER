#pragma once
#include <cstdint>
#include "SDL.h"
#include "Math.h"
class Component
{
public:
	Component(class Actor* owner, int updateOrder = 100);
	virtual ~Component();

	virtual void Update(float deltaTime);
	Actor* GetOwner() const { return mOwner; }
	int GetUpdateOrder() const { return mUpdateOrder; }
	virtual void ProcessInput(const uint8_t* keyState){}

	virtual void Draw(SDL_Renderer* renderer  , const Vector2 &cameraPos) {}
	virtual void ProcessEvent(const SDL_Event& event) {}

protected:
	class Actor* mOwner;
	int mUpdateOrder;
};