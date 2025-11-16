#pragma once
#include <vector>
#include "Math.h"
#include <cstdint>
#include "HPBarComponent.h"
using namespace std;

class Actor
{
public:
	enum State {
		EAlive,
		EPaused,
		EStoping,
		EStop
	};

	Actor(class Game* game);
	virtual ~Actor();

	void Update(float deltaTime);
	void UpdateComponents(float deltaTime);
	virtual void UpdateActor(float deltaTime);
	const std::vector<Component*>& GetComponents() const { return mComponents; }
	void AddComponent(class Component* component);
	void RemoveComponent(Component* component);

	const Vector2& GetPosition() const { return mPosition; }
	void SetPosition(const Vector2& pos) { mPosition = pos; }
	float GetRotation() const { return mRotation; }
	void SetRotation(float rotation) { mRotation = rotation; }
	float GetScale() const { return mScale; }
	void SetScale(float scale) { mScale = scale; }
	State GetState() const { return mState; }
	void SetState(State state) { mState = state; }

	class Game* GetGame() { return mGame; }

	Vector2 GetForward() const {
		return Vector2(Math::Cos(mRotation), Math::Sin(mRotation));
	}

	Vector2 GetRight() const {
		return Vector2(-Math::Sin(mRotation), Math::Cos(mRotation));
	}


	//«««•W€“ü—Íˆ——p«««
	void ProcessInput(const uint8_t* keyState);
	//«««“Æ©“ü—Íˆ——p«««
	virtual void ActorInput(const uint8_t* keyState);

private:
	State mState;
	Vector2 mPosition;
	float mScale;
	float mRotation;

	vector<class Component*>mComponents;
	class Game* mGame;
};