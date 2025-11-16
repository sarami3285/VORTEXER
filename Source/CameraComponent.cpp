#include "CameraComponent.h"
#include "Actor.h"

float CameraComponent::mMapWidth = 0.0f;
float CameraComponent::mMapHeight = 0.0f;

CameraComponent::CameraComponent(Actor* owner)
    : Component(owner)
{
    Vector2 pos = owner->GetPosition();
    mCameraPos = pos;
}

void CameraComponent::Update(float deltaTime)
{
    Vector2 playerPos = mOwner->GetPosition();

    mCameraPos.x = playerPos.x - mScreenWidth / 2.0f;
    mCameraPos.y = playerPos.y - mScreenHeight / 2.0f;

    if (mCameraPos.x < 0) mCameraPos.x = 0;
    if (mCameraPos.y < 0) mCameraPos.y = 0;
    if (mCameraPos.x > mMapWidth - mScreenWidth) mCameraPos.x = mMapWidth - mScreenWidth;
    if (mCameraPos.y > mMapHeight - mScreenHeight) mCameraPos.y = mMapHeight - mScreenHeight;
}

