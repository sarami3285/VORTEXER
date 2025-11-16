#include "CollisionComponent.h"
#include "Actor.h"

CollisionComponent::CollisionComponent(class Actor* owner)
	:Component(owner)
	, mRadius(0.0f)
{

}

const Vector2& CollisionComponent::GetCenter() const
{
	return mOwner->GetPosition();
}

float CollisionComponent::GetRadius() const
{
	return mOwner->GetScale() * mRadius;
}

bool Intersect(const CollisionComponent& a, const CollisionComponent& b)
{
	Vector2 diff = a.GetCenter() - b.GetCenter();
	float distSq = diff.LengthSq();

	float radiiSq = a.GetRadius() + b.GetRadius();
	radiiSq *= radiiSq;

	return distSq <= radiiSq;
}