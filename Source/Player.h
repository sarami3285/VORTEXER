#pragma once
#include "Actor.h"
#include "SpriteComponent.h"
#include "InputComponent.h"
#include "vector"
#include "LowLevelEnemy.h"
#include "CameraComponent.h"
#include "CollisionComponent.h"
#include "WeaponComponent.h"

class LockOnEffect;

enum class AimMode {
	Auto,
	Manual
};

class Player : public Actor
{
public:
	Player(class Game* game);

	void UpdateActor(float deltaTime) override;
	void ActorInput(const uint8_t* keyState) override;
	void TakeDamage(int damage);

	void SetWeapon(const WeaponData& weapon);
	const WeaponData& GetWeapon() const;
	const std::string& GetCurrentWeaponName() const;

	LowLevelEnemy* GetNearestEnemy(const std::vector<Actor*>& actors);
	Vector2 ScreenToWorld(int screenX, int screenY);
	CameraComponent* GetCameraComponent() const { return mCameraComponent; }
	class CollisionComponent* GetCircle() { return mCollision; }
	HPComponent* mHPComponent;
	InputComponent* ic;

	LowLevelEnemy* GetCurrentLockTarget() const { return mCurrentLockTarget; }
	AimMode GetAimMode() const { return mAimMode; }

	WeaponComponent* GetWeaponComponent() const { return mWeaponComponent; }
private:
	void UpdateSpriteBasedOnDirection(const Vector2& targetPos);

	float lockOnRange = 350.0f;
	bool prevTabState = false;


	WeaponData CurrentWeapon;
	WeaponData nextWeapon;
	WeaponComponent* mWeaponComponent;
	CollisionComponent* mCollision;
	SpriteComponent *mSpriteComponent;
	LockOnEffect* mLockOnEffect = nullptr;
	LowLevelEnemy* mCurrentLockTarget = nullptr;
	AimMode mAimMode;
	CameraComponent* mCameraComponent;
};