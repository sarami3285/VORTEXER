#pragma once
#include "Component.h"
#include "CameraComponent.h"

class MoveComponent : public Component {
public:
	MoveComponent(class Actor*owner, int updateOrder = 100);

	void Update(float deltaTime) override;

	float GetAngularSpeed() const { return mAngularSpeed; }
	float GetForwardSpeed() const { return mForwardSpeed; }
	float GetRightSpeed() const { return mRightSpeed; }
	void SetAngularSpeed(float speed) { mAngularSpeed = speed; }
	void SetForwardSpeed(float speed) { mForwardSpeed = speed; }
	void SetRightSpeed(float speed) { mRightSpeed = speed; }

private:
	CameraComponent* mCameraComponent;
	float mAngularSpeed;
	float mForwardSpeed;
	float mRightSpeed;
protected:
	float mMaxForwardSpeed;
	float mMaxRightSpeed;
};