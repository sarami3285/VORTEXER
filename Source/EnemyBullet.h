#pragma once
#include "Actor.h"
#include "AudioComponent.h"

class EnemyBullet : public Actor {
public:
	EnemyBullet(class Game* game);
	void UpdateActor(float deltaTime) override;

private:
	int Damage = 10;
	class CollisionComponent* mCollision;
	AudioComponent* mAudioComponent;
	float mDeathTimer;
};