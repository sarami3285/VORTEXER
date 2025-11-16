#include "Player.h"
#include "SpriteComponent.h"
#include "InputComponent.h"
#include "Game.h"
#include "Bullet.h"
#include "LowLevelEnemy.h"
#include "LockOnComponent.h"
#include "HPComponent.h"

#include "DeathEffect.h"
#include "Scenes.h"

Player::Player(Game* game)
    : Actor(game)
    , mAimMode(AimMode::Auto)
    , mCurrentLockTarget(nullptr)
    , mLockOnEffect(nullptr)
{
    game->SetPlayer(this);

    mSpriteComponent = new SpriteComponent(this, 1000);
    mSpriteComponent->SetTexture(game->GetTexture("Assets/PlayerBack.png"));
    mSpriteComponent->SetColor(1.0f, 1.0f, 1.0f);
    mSpriteComponent->SetCastShadow(true);

    mCollision = new CollisionComponent(this);
    mCollision->SetRadius(15.0f);

    mCameraComponent = new CameraComponent(this);

    mWeaponComponent = new WeaponComponent(this);

    mCameraComponent->SetMapSize(3240.0f, 3240.0f);
    game->SetCamera(mCameraComponent);

    mHPComponent = new HPComponent(this, 100);

    ic = new InputComponent(this);

    ic->SetForwardKey(SDL_SCANCODE_W);
    ic->SetBackKey(SDL_SCANCODE_S);
    ic->SetRightKey(SDL_SCANCODE_A);
    ic->SetLeftKey(SDL_SCANCODE_D);
    ic->SetBoostKey(SDL_SCANCODE_LSHIFT);
    ic->SetMaxForwardSpeed(300.0f);
    ic->SetMaxRightSpeed(300.0f);
}

void Player::UpdateActor(float deltaTime)
{
    if (mAimMode == AimMode::Auto)
    {
        LowLevelEnemy* nearest = GetNearestEnemy(GetGame()->GetActors());
        if (nearest)
        {
            float distSq = (nearest->GetPosition() - GetPosition()).LengthSq();
            if (distSq > lockOnRange * lockOnRange)
            {
                nearest = nullptr;
            }
        }

        if (nearest != mCurrentLockTarget)
        {
            if (mLockOnEffect)
            {
                mLockOnEffect->SetState(Actor::State::EStop);
                mLockOnEffect = nullptr;
            }
            if (nearest)
            {
                mLockOnEffect = new LockOnEffect(GetGame(), nearest);
            }
            mCurrentLockTarget = nearest;
        }
        if (mCurrentLockTarget)
        {
            UpdateSpriteBasedOnDirection(mCurrentLockTarget->GetPosition());
        }
    }
    else // マウスモード
    {
        mCurrentLockTarget = nullptr;

        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);
        Vector2 worldMousePos = ScreenToWorld(mouseX, mouseY);
        UpdateSpriteBasedOnDirection(worldMousePos);

        if (mLockOnEffect)
        {
            mLockOnEffect->SetPosition(worldMousePos);
            mLockOnEffect->SetTarget(nullptr);
        }
    }
}

void Player::ActorInput(const uint8_t* keyState)
{
    bool isTabPressed = keyState[SDL_SCANCODE_TAB];
    if (isTabPressed && !prevTabState)
    {
        mAimMode = (mAimMode == AimMode::Auto) ? AimMode::Manual : AimMode::Auto;
        if (mLockOnEffect)
        {
            mLockOnEffect->SetState(Actor::State::EStop);
            mLockOnEffect = nullptr;
        }
        SDL_Log("AimMode is changed");
        if (mAimMode == AimMode::Manual)
        {
            mLockOnEffect = new LockOnEffect(GetGame(), nullptr);
        }
    }
    prevTabState = isTabPressed;

    if (keyState[SDL_SCANCODE_SPACE])
    {
        Vector2 fireDir;

        if (mAimMode == AimMode::Auto)
        {
            LowLevelEnemy* nearest = GetNearestEnemy(GetGame()->GetActors());
            if (nearest)
            {
                float distSq = (nearest->GetPosition() - GetPosition()).LengthSq();
                if (distSq <= lockOnRange * lockOnRange)
                {
                    fireDir = nearest->GetPosition() - GetPosition();
                }
                else
                {
                    return;
                }
            }
            else
            {
                return;
            }
        }
        else
        {
            int mouseX, mouseY;
            SDL_GetMouseState(&mouseX, &mouseY);
            Vector2 worldMousePos = ScreenToWorld(mouseX, mouseY);
            fireDir = worldMousePos - GetPosition();
        }

        if (fireDir.LengthSq() > 0.0f)
        {
            fireDir.Normalize();
            mWeaponComponent->Fire(fireDir);
        }
    }
}

LowLevelEnemy* Player::GetNearestEnemy(const std::vector<Actor*>& actors)
{
    LowLevelEnemy* nearest = nullptr;
    float bestDistSq = FLT_MAX;
    Vector2 myPos = GetPosition();

    for (auto actor : actors)
    {
        if (auto enemy = dynamic_cast<LowLevelEnemy*>(actor))
        {
            float distSq = (enemy->GetPosition() - myPos).LengthSq();
            if (distSq < bestDistSq)
            {
                bestDistSq = distSq;
                nearest = enemy;
            }
        }
    }
    return nearest;
}

void Player::UpdateSpriteBasedOnDirection(const Vector2& targetPos)
{
    Vector2 direction = targetPos - GetPosition();
    if (direction.LengthSq() > 0.0f)
    {
        direction.Normalize();
        float angle = Math::Atan2(-direction.y, direction.x);  // ラジアン角

        if (angle >= -Math::Pi / 4 && angle < Math::Pi / 4)
        {
            mSpriteComponent->SetTexture(GetGame()->GetTexture("Assets/PlayerRight.png"));
        }
        else if (angle >= Math::Pi / 4 && angle < 3 * Math::Pi / 4)
        {
            mSpriteComponent->SetTexture(GetGame()->GetTexture("Assets/PlayerForward.png"));
        }
        else if (angle >= -3 * Math::Pi / 4 && angle < -Math::Pi / 4)
        {
            mSpriteComponent->SetTexture(GetGame()->GetTexture("Assets/PlayerBack.png"));
        }
        else
        {
            mSpriteComponent->SetTexture(GetGame()->GetTexture("Assets/PlayerLeft.png"));
        }
    }
}

Vector2 Player::ScreenToWorld(int screenX, int screenY)
{
    Vector2 camPos = GetGame()->GetCamera()->GetCameraPos();
    Vector2 worldPos = Vector2(float(screenX), float(screenY)) + camPos;

    return worldPos;
}

void Player::TakeDamage(int damage) {
    mHPComponent->TakeDamage(damage);
    if (mHPComponent->IsDead())
    {
        SDL_Log("GameOver ( ﾟДﾟ)");
        DeathEffect* effect = new DeathEffect(GetGame());
        effect->SetPosition(GetPosition());
        GetGame()->AddActor(effect);
        

        GetGame()->RequestSceneChange(std::make_unique<MenuScene>(GetGame(), GetGame()->GetRenderer()));
    }
}

void Player::SetWeapon(const WeaponData& weapon)
{
    SDL_Log("SetWeapon start: %s", weapon.name.c_str());

    CurrentWeapon = weapon;
    SDL_Log("Weapon copied");

    if (mWeaponComponent)
    {
        SDL_Log("WeaponComponent exists");
        mWeaponComponent->SetWeaponData(weapon);
        mWeaponComponent->SetCurrentWeaponName(weapon.name);
        SDL_Log("Weapon set to: %s", weapon.name.c_str());
    }
    else
    {
        SDL_Log("WeaponComponent is nullptr!");
    }
}


const WeaponData& Player::GetWeapon() const
{
    return CurrentWeapon;
}

const std::string& Player::GetCurrentWeaponName() const
{
    return CurrentWeapon.name;
}