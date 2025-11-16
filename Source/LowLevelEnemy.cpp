#include "LowLevelEnemy.h"
#include "Random.h"
#include "SpriteComponent.h"
#include "MoveComponent.h"
#include "Game.h"
#include "CollisionComponent.h"
#include "HPComponent.h"
#include "HPBarComponent.h"
#include "Player.h"
#include "CameraComponent.h"
#include "EnemyBullet.h"
#include "DeathEffect.h"

LowLevelEnemy::LowLevelEnemy(Game* game)
    : Actor(game)
    , mCollision(nullptr)
    , mMoveComponent(nullptr)
    , mWaitTimer(-1.0f)
    , mSpriteComponent(nullptr)
{
    mHPComponent = new HPComponent(this, 50);

    Vector2 minPos(MapOffset, MapOffset);
    Vector2 maxPos(CameraComponent::mMapWidth - MapOffset, CameraComponent::mMapHeight - MapOffset);

    Vector2 randomPos = GetRandomPositionInRange(minPos, maxPos);
    SetPosition(randomPos);

    SetRotation(Random::GetFloatRange(0.0f, Math::TwoPi));

    mMoveComponent = new MoveComponent(this);
    mMoveComponent->SetForwardSpeed(50.0f);

    mSpriteComponent = new SpriteComponent(this);
    mSpriteComponent->SetTexture(game->GetTexture("Assets/tank1.png"));
    mSpriteComponent->SetCastShadow(true);

    mCollision = new CollisionComponent(this);
    mCollision->SetRadius(40.0f);

    mPatrolCenter = GetPosition();
    mPatrolRadius = 200.0f;

    mTargetPos = GetRandomPositionInRange(mPatrolCenter - Vector2(mPatrolRadius, mPatrolRadius),
        mPatrolCenter + Vector2(mPatrolRadius, mPatrolRadius));

    new HPBarComponent(this, mHPComponent);

    mPlayer = game->GetPlayer();
}

LowLevelEnemy::~LowLevelEnemy() {
    if (mHPBarSprite) {
        delete mHPBarSprite;
        mHPBarSprite = nullptr;
    }
}

void LowLevelEnemy::SetHPBar(HUDSpriteComponent* sprite) {
    mHPBarSprite = sprite;
}

void LowLevelEnemy::UpdateActor(float dt) {
    if (mSpriteComponent && mHPComponent) {
        float hpRatio = static_cast<float>(mHPComponent->GetHP()) / mHPComponent->GetMaxHP();

        float rValue = 1.0f;
        float gValue = hpRatio; 
        float bValue = hpRatio;
        mSpriteComponent->SetColor(rValue, gValue, bValue);
    }

    Patrol(dt);

    if (mHPComponent->GetHP() <= 0) {
        SetState(EStop);
        if (mHPBarSprite) {
            mHPBarSprite->GetOwner()->RemoveComponent(mHPBarSprite);
        }
    }
}

void LowLevelEnemy::Patrol(float dt)
{
    const float waitDuration = 5.0f;
    const float reachThreshold = 10.0f;

    Vector2 currentPos = GetPosition();
    Vector2 mPlayerPos = mPlayer->GetPosition();
    Vector2 toTarget = mTargetPos - currentPos;
    float distance = toTarget.Length();
    float surchRange = 450.0f;

    if ((mPlayerPos - currentPos).Length() <= surchRange) {
        Attack(dt);
    }
    else {
        if (distance < reachThreshold)
        {
            if (mWaitTimer <= 0.0f)
            {
                if (mMoveComponent)
                {
                    mMoveComponent->SetForwardSpeed(0.0f);
                }
                mWaitTimer = waitDuration;
            }
            else
            {
                mWaitTimer -= dt;

                if (mWaitTimer <= 0.0f)
                {
                    Vector2 minPos = mPatrolCenter - Vector2(mPatrolRadius, mPatrolRadius);
                    Vector2 maxPos = mPatrolCenter + Vector2(mPatrolRadius, mPatrolRadius);
                    mTargetPos = GetRandomPositionInRange(minPos, maxPos);

                    if (mMoveComponent)
                    {
                        mMoveComponent->SetForwardSpeed(50.0f);
                    }
                }
                else
                {
                    if (mMoveComponent)
                    {
                        mMoveComponent->SetForwardSpeed(0.0f);
                    }
                }
            }
        }
        else
        {
            mWaitTimer = 0.0f;
            float targetAngle = atan2(toTarget.y, toTarget.x);
            SetRotation(targetAngle);

            if (mMoveComponent)
            {
                mMoveComponent->SetForwardSpeed(50.0f);
            }
        }
    }
}

void LowLevelEnemy::Attack(float dt) {
    if (!mPlayer || !mMoveComponent) return;

    Vector2 toPlayer = mPlayer->GetPosition() - GetPosition();
    float dist = toPlayer.Length();
    float attackRange = 300.0f;
    SetRotation(atan2(toPlayer.y, toPlayer.x));

    if (dist > attackRange)
    {
        mMoveComponent->SetForwardSpeed(100.0f);
    }
    else
    {
        mMoveComponent->SetForwardSpeed(0.0f);
        

        mFireCooldown -= dt;
        if (mFireCooldown <= 0.0f)
        {
            toPlayer.Normalize();
            float rotation = atan2(toPlayer.y, toPlayer.x);

            EnemyBullet* bullet = new EnemyBullet(GetGame());
            bullet->SetPosition(GetPosition());
            bullet->SetRotation(rotation);

            mFireCooldown = 1.0f;
        }
    }
}

void LowLevelEnemy::TakeDamage(int amount)
{
    mHPComponent->TakeDamage(amount);
    if (mHPComponent->IsDead())
    {
        GetGame()->GetDataManager().AddCurrency(100);

        DeathEffect* effect = new DeathEffect(GetGame());
        effect->SetPosition(GetPosition());
        GetGame()->AddActor(effect);
        SetState(EStop);
    }
}

Vector2 LowLevelEnemy::GetRandomPositionInRange(const Vector2& minPos, const Vector2& maxPos)
{
    float x = Random::GetFloatRange(minPos.x, maxPos.x);
    float y = Random::GetFloatRange(minPos.y, maxPos.y);
    return Vector2(x, y);
}
