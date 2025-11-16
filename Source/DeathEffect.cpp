#include "DeathEffect.h"
#include "Game.h"

DeathEffect::DeathEffect(Game* game)
    : Actor(game)
    , mFrameDuration(0.2f)
    , mFrameTimer(0.0f)
    , mCurrentFrame(0)
    , mWaitTimer(0.0f)
{
    mSprite = new SpriteComponent(this);
    SDL_Texture* tex = game->GetTexture("Assets/explosion.png");
    mSprite->SetTexture(tex);

    mAudioComponent = new AudioComponent(this);
    mAudioComponent->LoadSE("death", "Assets/Audio/game_explosion7.mp3");

    // ここでサイズ取得してピクセル単位でUV設定
    float frameWidth = mSprite->GetTexWidth() / static_cast<float>(4);
    float frameHeight = static_cast<float>(mSprite->GetTexHeight());

    mSprite->SetSpriteUV(0, 0, frameWidth, frameHeight);
    SetState(EStoping);

    mAudioComponent->PlaySE("death");
}


void DeathEffect::UpdateActor(float deltaTime)
{
    mFrameTimer += deltaTime;
    if (mFrameTimer >= mFrameDuration)
    {
        mFrameTimer -= mFrameDuration;
        mCurrentFrame++;

        if (mCurrentFrame >= mTotalFrames)
        {
            SetState(EStop);
            return;
        }

        float frameWidth = mSprite->GetTexWidth() / static_cast<float>(4);
        float frameHeight = static_cast<float>(mSprite->GetTexHeight());
        float u = frameWidth * mCurrentFrame;

        mSprite->SetSpriteUV(u, 0, frameWidth, frameHeight);
    }
}
