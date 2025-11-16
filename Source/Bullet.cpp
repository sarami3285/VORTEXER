#include "Bullet.h"
#include "SpriteComponent.h"
#include "MoveComponent.h"
#include "CollisionComponent.h"
#include "LowLevelEnemy.h"
#include "Game.h"
#include "Random.h"

Bullet::Bullet(Game* game)
	: Actor(game)
	, mSpriteComponent(nullptr)
{
	SpriteComponent* sc = new SpriteComponent(this);
	sc->SetTexture(game->GetTexture("Assets/Bullet.png"));
	mSpriteComponent = sc;

	MoveComponent* mc = new MoveComponent(this);
	mc->SetForwardSpeed(800.0f);
	mMoveComponent = mc;

	mCollision = new CollisionComponent(this);
	mCollision->SetRadius(11.0f);

	Damage = 12;
	maxTimer = 1.0f;
	deathTimer = maxTimer;
	rate = 1.0f;//rate を大きくすると減衰が急激になる。rate を小さくすると減衰が緩やかになる。二次関数的に推移
}


void Bullet::SetSpeed(float speed)
{
	if (mMoveComponent)
	{
		mMoveComponent->SetForwardSpeed(speed);
	}
}

void Bullet::UpdateActor(float deltaTime) {

	// 跳弾タイマーを管理する定数と閾値
	const float MAX_RICOCHET_TIME = 0.5f; // 0.5秒の寿命
	const float RICOSHET_THRESHOLD = Damage / 2.0f;
	const float MAX_OFFSET_RAD = Math::Pi / 3.0f; // 45度

	// --- 1. 跳弾中の時間管理 ---
	if (mIsRicocheting) {
		mRicochetTimer -= deltaTime;
		if (mRicochetTimer <= 0.0f) {
			SetState(EStop); // 0.5秒経過したら消滅
			return;
		}
	}

	// --- 2. ダメージ減衰と自然死の管理（未衝突の場合のみ） ---
	if (!mIsRicocheting) {
		deathTimer -= deltaTime;
		if (deathTimer <= 0.0f) {
			SetState(EStop); // 寿命が尽きたら消滅
			return;
		}
	}

	float effectiveDamage = Damage * powf((deathTimer / maxTimer), rate);

	// --- 3. 視覚化（威力減衰に応じた透明度の変化） ---
	if (mSpriteComponent) {
		// 威力の割合 (0.0f ～ 1.0f) を直接アルファ値にマッピング
		float damageRatio = effectiveDamage / Damage;
		float alphaValue = damageRatio;

		if (alphaValue < 0.0f) {
			alphaValue = 0.0f;
		}

		mSpriteComponent->SetAlpha(alphaValue);
	}

	// --- 4. 境界チェック ---
	Vector2 pos = GetPosition();
	if (pos.x <= 0.0f || pos.x >= CameraComponent::mMapWidth ||
		pos.y <= 0.0f || pos.y >= CameraComponent::mMapHeight)
	{
		SetState(EStop);
		return;
	}

	// --- 5. 衝突判定（跳弾中ではない場合のみ実行） ---
	if (!mIsRicocheting) {
		for (auto actor : GetGame()->GetActors())
		{
			if (auto enemy = dynamic_cast<LowLevelEnemy*>(actor))
			{
				if (enemy->GetState() == Actor::EAlive && Intersect(*mCollision, *(enemy->GetCircle())))
				{
					// 敵には必ずダメージを与える
					enemy->TakeDamage(effectiveDamage);

					// --- 衝突が発生した ---
					if (effectiveDamage < RICOSHET_THRESHOLD)
					{
						// A. 跳弾ルート (0.5秒生存)

						// 1. 跳弾フラグをオンにし、タイマーを設定
						mIsRicocheting = true;
						mRicochetTimer = MAX_RICOCHET_TIME;

						// 2. 角度を反転 (180度) ＋ ランダムオフセット (+-45度)
						float randomOffset = Random::GetFloat() * (2.0f * MAX_OFFSET_RAD) - MAX_OFFSET_RAD;
						float newRotation = GetRotation() + Math::Pi + randomOffset;
						SetRotation(newRotation);

						// 弾は消滅せず、mRicochetTimerの終了を待つ
						break; // 衝突検出ループを抜ける
					}
					else
					{
						// B. 通常ヒットルート (即時消滅)
						SetState(EStop);
						break; // 衝突検出ループを抜ける
					}
				}
			}
		}
	}
}