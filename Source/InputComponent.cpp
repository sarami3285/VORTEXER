#include "InputComponent.h"
#include "Actor.h"
#include "CameraComponent.h"


InputComponent::InputComponent(class Actor* owner)
    : MoveComponent(owner)
    , mForwardKey(0)
    , mBackKey(0)
    , mRightKey(0)
    , mLeftKey(0)
    , mBoostKey(0)
    , IsForward(false)
    , IsBack(false)
    , IsRight(false)
    , IsLeft(false)

    , mIsBoosting(false)
    , mBoostMultiplier(1.5f)
    , mBoostDuration(2.0f)
    , mBoostTimer(2.0f)
    , mBoostRecoveryRate(0.5f)

    , mTargetForwardSpeed(0.0f)
    , mTargetRightSpeed(0.0f)
    , mCurrentForwardSpeed(0.0f)
    , mCurrentRightSpeed(0.0f)
    , mNormalAcceleration(1500.0f)
    , mBoostAcceleration(2600.0f)
    , mDeceleration(600.0f)
{
}

void InputComponent::ProcessInput(const uint8_t* keyState)
{
    InputClear();

    if (keyState[mBoostKey] && mBoostTimer > 0.0f) {
        mIsBoosting = true;
    }
    else {
        mIsBoosting = false;
    }

    float currentMaxForward = mIsBoosting ? mMaxForwardSpeed * mBoostMultiplier : mMaxForwardSpeed;
    float currentMaxRight = mIsBoosting ? mMaxRightSpeed * mBoostMultiplier : mMaxRightSpeed;

    if (keyState[mForwardKey]) {
        mTargetForwardSpeed = -currentMaxForward;
        IsForward = true;
    }
    else if (keyState[mBackKey]) {
        mTargetForwardSpeed = currentMaxForward;
        IsBack = true;
    }
    else {
        mTargetForwardSpeed = 0.0f;
    }

    if (keyState[mRightKey]) {
        mTargetRightSpeed = -currentMaxRight;
        IsRight = true;
    }
    else if (keyState[mLeftKey]) {
        mTargetRightSpeed = currentMaxRight;
        IsLeft = true;
    }
    else {
        mTargetRightSpeed = 0.0f;
    }
}


void InputComponent::Update(float deltaTime)
{
    if (mIsBoosting) {
        mBoostTimer -= deltaTime;
        if (mBoostTimer < 0.0f) {
            mBoostTimer = 0.0f;
            mIsBoosting = false;
        }
    }
    else {
        mBoostTimer += mBoostRecoveryRate * deltaTime;
        if (mBoostTimer > mBoostDuration) {
            mBoostTimer = mBoostDuration;
        }
    }

    // ブーストによる加速力をローカル変数 acceleration に格納（定義は一度のみ）
    // mNormalAcceleration や mBoostAcceleration がメンバ変数として定義されている前提
    float acceleration = mIsBoosting ? mBoostAcceleration : mNormalAcceleration;

    // Y軸方向（前後）の速度更新
    if (mCurrentForwardSpeed < mTargetForwardSpeed) {
        // 目標速度に向かって加速。キーを離して 0 に向かう場合は mDeceleration を使用。
        float rate = (mTargetForwardSpeed != 0.0f) ? acceleration : mDeceleration;
        mCurrentForwardSpeed += rate * deltaTime;
        if (mCurrentForwardSpeed > mTargetForwardSpeed)
            mCurrentForwardSpeed = mTargetForwardSpeed;
    }
    else if (mCurrentForwardSpeed > mTargetForwardSpeed) {
        // 目標速度に向かって減速。キーを離して 0 に向かう場合は mDeceleration を使用。
        float rate = (mTargetForwardSpeed != 0.0f) ? acceleration : mDeceleration;
        mCurrentForwardSpeed -= rate * deltaTime;
        if (mCurrentForwardSpeed < mTargetForwardSpeed)
            mCurrentForwardSpeed = mTargetForwardSpeed;
    }

    // X軸方向（左右）の速度更新
    if (mCurrentRightSpeed < mTargetRightSpeed) {
        float rate = (mTargetRightSpeed != 0.0f) ? acceleration : mDeceleration;
        mCurrentRightSpeed += rate * deltaTime;
        if (mCurrentRightSpeed > mTargetRightSpeed)
            mCurrentRightSpeed = mTargetRightSpeed;
    }
    else if (mCurrentRightSpeed > mTargetRightSpeed) {
        float rate = (mTargetRightSpeed != 0.0f) ? acceleration : mDeceleration;
        mCurrentRightSpeed -= rate * deltaTime;
        if (mCurrentRightSpeed < mTargetRightSpeed)
            mCurrentRightSpeed = mTargetRightSpeed;
    }


    Vector2 pos = mOwner->GetPosition();
    pos.y += mCurrentForwardSpeed * deltaTime;
    pos.x += mCurrentRightSpeed * deltaTime;

    if (pos.x < 25.0f) { pos.x = 25.0f; }
    else if (pos.x > CameraComponent::mMapWidth - 25) { pos.x = CameraComponent::mMapWidth - 25; }

    if (pos.y < 30.0f) { pos.y = 30.0f; }
    else if (pos.y > CameraComponent::mMapHeight - 30) { pos.y = CameraComponent::mMapHeight - 30; }

    mOwner->SetPosition(pos);
}

void InputComponent::InputClear()
{
    IsForward = false;
    IsBack = false;
    IsRight = false;
    IsLeft = false;
}

