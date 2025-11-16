#include "LockOnComponent.h"
#include "Game.h"

LockOnEffect::LockOnEffect(Game* game, Actor* target)
    : Actor(game), mTarget(target)
{
    mSprite = new SpriteComponent(this,250);
    mSprite->SetTexture(game->GetTexture("Assets/LockOn.png"));
}

void LockOnEffect::UpdateActor(float deltaTime)
{
    if (mTarget)
    {
        SetPosition(mTarget->GetPosition() + mOffset);
    }
}

Actor* LockOnEffect::GetTarget() const
{
    return mTarget;
}