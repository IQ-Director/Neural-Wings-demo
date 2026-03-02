#pragma once
#include "Engine/Math/Math.h"
#include "Engine/Core/Components/Components.h"
class GameObject;
class GameWorld;

struct mRaycastHit
{
    bool hit = false;
    float distance = 0.f;
    Vector3f point;
    Vector3f normal;
    GameObject *entity = nullptr;
};

class mRay
{
public:
    Vector3f origin;
    Vector3f direction;

    mRay() : origin(0, 0, 0), direction(0, 0, 1) {}
    mRay(const Vector3f &origin, const Vector3f &direction) : origin(origin), direction(direction.Normalized()) {}
    mRaycastHit Raycast(float maxDistance, GameWorld &world, GameObject *ignoreEntity = nullptr) const;

private:
    bool IntersectOBB(const TransformComponent &tf, const RigidbodyComponent &rb, float &outDist, Vector3f &outNormal) const;
    bool IntersectSphere(const TransformComponent &tf, const RigidbodyComponent &rb, float &outDist, Vector3f &outNormal) const;
};
