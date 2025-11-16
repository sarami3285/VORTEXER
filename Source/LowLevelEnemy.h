#pragma once
#include "Actor.h"
#include "Math.h"
#include "AudioComponent.h"
class Player;
class HUDSpriteComponent;

class LowLevelEnemy : public Actor
{
public:
	LowLevelEnemy(class Game* game);
	~LowLevelEnemy();
	void UpdateActor(float deltaTime) override;
	void TakeDamage(int amount);
	void SetHPBar(HUDSpriteComponent* sprite);

	class CollisionComponent* GetCircle() { return mCollision; }
private:
	void Patrol(float dt);
	void Attack(float dt);
	Vector2 GetRandomPositionInRange(const Vector2& minPos, const Vector2& maxPos);

	class CollisionComponent* mCollision;
	class MoveComponent* mMoveComponent;
	class HPComponent* mHPComponent;
	SpriteComponent* mSpriteComponent;
	HUDSpriteComponent* mHPBarSprite = nullptr;

	Player* mPlayer;
	Vector2 mTargetPos;
	Vector2 mPatrolCenter;
	float MapOffset = 250.0f; //パトロール範囲が画面外にならないように
	float mPatrolRadius;
	float mWaitTimer;
	float mFireCooldown = 0.0f;

	bool mIsDamaged = false;
	Vector3 mFixedColor = Vector3(1.0f, 1.0f, 1.0f);
};