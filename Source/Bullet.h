#pragma once
#include "Actor.h"

class Bullet : public Actor {
public:
	Bullet(class Game* game);
	void UpdateActor(float deltaTime) override;

	void SetBaseDamage(int damage) { Damage = damage; }
	void SetMaxLifeTime(float time) { maxTimer = time; deathTimer = time; }
	void SetDecayRate(float decay) { rate = decay; }
	void SetSpeed(float speed);

	class SpriteComponent* GetSprite() const { return mSpriteComponent; }

private:
	int Damage;
	float maxTimer;
	float deathTimer;

	bool mIsRicocheting;
	float mRicochetTimer;

	float rate;
	class CollisionComponent* mCollision;
	class SpriteComponent* mSpriteComponent;
	class MoveComponent* mMoveComponent;
};
