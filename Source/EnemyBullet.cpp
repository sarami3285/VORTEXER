#include "EnemyBullet.h"
#include "SpriteComponent.h"
#include "MoveComponent.h"
#include "CollisionComponent.h"
#include "Player.h"
#include "Game.h"

EnemyBullet::EnemyBullet(Game* game)
	:Actor(game)
	, mDeathTimer(1.0f)
{
	SpriteComponent* sc = new SpriteComponent(this);
	sc->SetTexture(game->GetTexture("Assets/Bullet.png"));

	MoveComponent* mc = new MoveComponent(this);
	mc->SetForwardSpeed(500.0f);

	mAudioComponent = new AudioComponent(this);
	mAudioComponent->LoadSE("shot", "Assets/Audio/shotgun.mp3");

	mCollision = new CollisionComponent(this);
	mCollision->SetRadius(11.0f);

	SetPosition(Vector2::Zero);

	mAudioComponent->PlaySE("shot");
}

void EnemyBullet::UpdateActor(float deltaTime) {
	mDeathTimer -= deltaTime;
	if (mDeathTimer <= 0.0f)
	{
		SetState(EStop);
	}
	else
	{
		Vector2 pos = GetPosition();
		if (pos.x <= 0.0f || pos.x >= CameraComponent::mMapWidth ||
			pos.y <= 0.0f || pos.y >= CameraComponent::mMapHeight)
		{
			SetState(EStop);
		}


		Player* player = GetGame()->GetPlayer();
		if (player && Intersect(*mCollision, *(player->GetCircle())))
		{
			SetState(EStop);
			player->TakeDamage(Damage);
		}
	}
}