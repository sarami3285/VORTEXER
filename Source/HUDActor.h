#pragma once
#include "Actor.h"
#include "MiniMapComponent.h"
#include "MiniMapSpriteComponent.h"

class HUDActor : public Actor {
public:
    HUDActor(Game* game)
        : Actor(game)
    {
        SetState(Actor::EAlive);
        mMiniMapLogic = new MiniMapComponent(this);
        mMiniMapDraw = new MiniMapSpriteComponent(this, mMiniMapLogic);
        mMiniMapLogic->SetRadius(90.0f);
        mMiniMapLogic->SetVisibleRange(1500.0f);
        mMiniMapDraw->SetOffset(Vector2(900.0f, 650.0f));
    }



    MiniMapComponent* GetMiniMapLogic() const { return mMiniMapLogic; }
    MiniMapSpriteComponent* GetMiniMapDraw() const { return mMiniMapDraw; }

private:
    MiniMapComponent* mMiniMapLogic;
    MiniMapSpriteComponent* mMiniMapDraw;
};