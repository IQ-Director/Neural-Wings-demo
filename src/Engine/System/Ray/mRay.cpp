#include "mRay.h"
#include "Engine/Core/Components/Components.h"
#include "Engine/Core/GameWorld.h"
#include <limits>

mRaycastHit mRay::Raycast(float maxDistance, GameWorld &world, GameObject *ignoreEntity) const
{
    mRaycastHit closestHit;
    closestHit.distance = std::numeric_limits<float>::max();
    auto entities = world.GetEntitiesWith<RigidbodyComponent, TransformComponent>();

    for (auto *entity : entities)
    {
        if (entity == ignoreEntity)
            continue;
        auto &rb = entity->GetComponent<RigidbodyComponent>();
        auto &tf = entity->GetComponent<TransformComponent>();

        if (!rb.Collidable)
            continue;

        float dist = 0.0f;
        Vector3f normal;

        if (rb.colliderType == ColliderType::BOX)
        {
            if (IntersectOBB(tf, rb, dist, normal))
            {
                if (dist > 0 && dist < maxDistance && dist < closestHit.distance)
                {
                    closestHit.hit = true;
                    closestHit.distance = dist;
                    closestHit.entity = entity;
                    closestHit.normal = normal;
                    closestHit.point = origin + direction * dist;
                }
            }
        }
        else if (rb.colliderType == ColliderType::SPHERE)
        {
            if (IntersectSphere(tf, rb, dist, normal))
            {
                if (dist > 0 && dist < maxDistance && dist < closestHit.distance)
                {
                    closestHit.hit = true;
                    closestHit.distance = dist;
                    closestHit.entity = entity;
                    closestHit.normal = normal;
                    closestHit.point = origin + direction * dist;
                }
            }
        }
        else
        {
            std::cerr << "[Ray]: Unknown collider type for entity: " << entity->GetName() << std::endl;
        }
        return closestHit;
    }
}

bool mRay::IntersectOBB(const TransformComponent &tf, const RigidbodyComponent &rb, float &outDist, Vector3f &outNormal) const
{
    Matrix4f invWorld = tf.GetWorldMatrix().inverse();
    Vector3f localOrigin = (invWorld * Vector4f(origin, 1.0f)).xyz();
    Vector3f localDir = ((invWorld * Vector4f(direction, 0.0f)).xyz()).Normalized();

    Vector3f boxMin = rb.localAABB.min;
    Vector3f boxMax = rb.localAABB.max;
    float tMin = 0.0f;
    float tMax = std::numeric_limits<float>::max();
    Vector3f tempNormal;
    for (size_t i = 0; i < 3; ++i)
    {
        if (std::abs(localDir[i]) < 1e-6f)
        {
            if (localOrigin[i] < boxMin[i] || localOrigin[i] > boxMax[i])
                return false;
        }
        else
        {
            float invD = 1.0f / localDir[i];
            float t1 = (boxMin[i] - localOrigin[i]) * invD;
            float t2 = (boxMax[i] - localOrigin[i]) * invD;

            Vector3f normalSide = Vector3f::ZERO;
            normalSide[i] = (t1 < t2) ? -1.0f : 1.0f;
            if (t1 > t2)
                std::swap(t1, t2);
            if (t1 > tMin)
            {
                tMin = t1;
                tempNormal = normalSide;
            }
            if (t2 < tMax)
                tMax = t2;

            if (tMin > tMax)
                return false;
        }
    }
    outDist = tMin;
    outNormal = tf.GetWorldRotation() * (tempNormal);
    return true;
}
bool mRay::IntersectSphere(const TransformComponent &tf, const RigidbodyComponent &rb, float &outDist, Vector3f &outNormal) const
{
    Vector3f localCenter = tf.GetWorldPosition();
    float radius = rb.boudingRadius;

    Vector3f oc = -localCenter + origin;
    float a = direction * (direction);
    float b = 2.0f * oc * (direction);
    float c = (oc * (oc)) - (radius * radius);
    float discriminant = b * b - 4 * a * c;

    if (discriminant < 0)
        return false;

    float t = (-b - sqrt(discriminant)) / (2.0f * a);
    if (t < 0)
        return false;

    outDist = t;
    outNormal = (origin + (direction * t) - localCenter).Normalized();
    return true;
}