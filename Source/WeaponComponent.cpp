#include "WeaponComponent.h"
#include "Game.h"
#include "Bullet.h"
#include "Actor.h"
#include "SpriteComponent.h"
#include "Random.h"

WeaponComponent::WeaponComponent(Actor* owner)
    : Component(owner)
    , mCooldown(0.0f)
{
    // 初期化用デフォルト武器
    mData.fireRate =0.3f;
    mData.baseDamage = 20;
    mData.bulletSpeed = 600.0f;
    mData.bulletTexturePath = "Assets/Bullet.png";
    mData.decayRate = 1.0f;

    mAudioComponent = new AudioComponent(owner);
    mAudioComponent->LoadSE("shot","Assets/Audio/shot.mp3");
}

void WeaponComponent::Update(float deltaTime)
{
    mCooldown -= deltaTime;

    if (mIsBurstFiring)
    {
        Player* player = dynamic_cast<Player*>(GetOwner());
        if (player && player->GetAimMode() == AimMode::Auto)
        {
            LowLevelEnemy* target = player->GetCurrentLockTarget();
            if (target)
            {
                Vector2 dir = target->GetPosition() - player->GetPosition();
                if (dir.LengthSq() > 0.0f)
                {
                    dir.Normalize();
                    mFireDirection = dir;
                }
            }
        }

        mBurstTimer -= deltaTime;
        if (mBurstTimer <= 0.0f && mBurstShotFired < mData.burstCount)
        {
            FireSingleShot(mFireDirection);
            mBurstShotFired++;
            mBurstTimer = mData.burstInterval;
        }

        if (mBurstShotFired >= mData.burstCount)
        {
            mIsBurstFiring = false;
            mCooldown = 1.0f / mData.fireRate;
            mAutoFire = false;
        }
    }
    else if (mAutoFire && mCooldown <= 0.0f)
    {
        StartBurstFire(mFireDirection);
    }
}


void WeaponComponent::StartBurstFire(const Vector2& direction)
{
    if (mIsBurstFiring || mCooldown > 0.0f) return;

    mIsBurstFiring = true;
    mBurstShotFired = 0;
    mBurstTimer = 0.0f;
    mFireDirection = direction;
}

void WeaponComponent::FireSingleShot(const Vector2& direction)
{
    mAudioComponent->PlaySE("shot");
    for (int i = 0; i < mData.pelletCount; ++i)
    {
        float angleOffset = 0.0f;
        if (mData.pelletCount > 1)
        {
            float spreadRad = Math::DegToRad(mData.spreadAngle);
            angleOffset = -spreadRad / 2 + (spreadRad / (mData.pelletCount - 1)) * i;
        }

        // ランダムブレ角度（度）を -randomSpread 〜 +randomSpread の範囲で取得
        float randomAngleDeg = Random::GetFloatRange(-mData.randomSpread, mData.randomSpread);
        float randomAngleRad = Math::DegToRad(randomAngleDeg);

        // 最終角度は均等スプレッド + ランダムブレ
        float finalAngle = angleOffset + randomAngleRad;

        Vector2 pelletDir = RotateVector(direction, finalAngle);

        Bullet* bullet = new Bullet(GetOwner()->GetGame());
        bullet->SetPosition(GetOwner()->GetPosition());
        bullet->SetRotation(Math::Atan2(pelletDir.y, pelletDir.x));
        bullet->SetBaseDamage(mData.baseDamage);
        bullet->SetSpeed(mData.bulletSpeed);
        bullet->SetDecayRate(mData.decayRate);
        bullet->SetMaxLifeTime(1.0f);
        bullet->GetSprite()->SetTexture(GetOwner()->GetGame()->GetTexture(mData.bulletTexturePath));

        SpriteComponent* bulletSprite = bullet->GetSprite();
        if (bulletSprite)
        {
            bulletSprite->SetTexture(GetOwner()->GetGame()->GetTexture(mData.bulletTexturePath));
        }
    }
}




void WeaponComponent::Fire(const Vector2& direction)
{
    StartBurstFire(direction);
}
