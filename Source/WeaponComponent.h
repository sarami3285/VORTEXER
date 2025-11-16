#pragma once
#include "Component.h"
#include "Math.h"
#include "WeaponData.h"
#include "AudioComponent.h"

class WeaponComponent : public Component
{
public:
    WeaponComponent(class Actor* owner);

    void Update(float deltaTime) override;
    void Fire(const Vector2& direction);
    void StartBurstFire(const Vector2& direction);
    void FireSingleShot(const Vector2& direction);

    void SetWeaponData(const WeaponData& data) { mData = data; }
    WeaponData GetWeaponData() { return mData; }

    void SetAutoFire(bool autoFire) { mAutoFire = autoFire; }
    void SetFireDirection(Vector2 dir) { mFireDirection = dir; }
    void SetCurrentWeaponName(const std::string& name) { mCurrentWeaponName = name; }
    const std::string& GetWeaponIconPath() const { return mData.weaponIcon; }
    const std::string& GetCurrentWeaponName() const { return mCurrentWeaponName; }


    bool LoadWeapon(const std::string& filepath, const std::string& weaponName);

private:
    AudioComponent* mAudioComponent;
    WeaponData mData;
    float mCooldown;
    bool mAutoFire = false;
    Vector2 mFireDirection;
    int mBurstShotFired = 0;
    float mBurstTimer = 0.0f;
    bool mIsBurstFiring = false;

    std::string mCurrentWeaponName;
};
