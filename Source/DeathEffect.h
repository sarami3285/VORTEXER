#pragma once
#include "Actor.h"
#include "SpriteComponent.h"
#include "AudioComponent.h"

class DeathEffect : public Actor
{
public:
    DeathEffect(class Game* game);
    void UpdateActor(float deltaTime) override;

private:
    SpriteComponent* mSprite;
    AudioComponent* mAudioComponent;
    float mFrameDuration;
    float mFrameTimer;
    int mCurrentFrame;
    float mWaitTimer;

    static constexpr int mTotalFrames = 4;
};

