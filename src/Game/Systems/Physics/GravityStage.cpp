#include "GravityStage.h"
#include "Engine/Core/GameWorld.h"
#include "Engine/Core/Components/RigidBodyComponent.h"
#include "Engine/Core/Components/TransformComponent.h"
#include <iostream>

GravityStage::GravityStage(Vector3f gravity) : m_gravity(gravity) {}

void GravityStage::Execute(GameWorld &world, float fixedDeltaTime)
{
    auto &gameObjects = world.GetGameObjects();
    if (gameObjects.empty())
    {
        std::cout << "[Gravity Stage]:Empty Game World" << std::endl;
        return;
    }
    for (auto &gameObject : gameObjects)
    {
        if (gameObject->HasComponent<RigidbodyComponent>())
        {
            auto &rb = gameObject->GetComponent<RigidbodyComponent>();
            if (rb.mass <= 0.001f)
                continue;
            auto &tf = gameObject->GetComponent<TransformComponent>();
            rb.AddForce(m_gravity * rb.mass);
            Vector3f corners[8];
            BoundingBox aabb = gameObject->GetWorldBoundingBox(&corners);
            float lowy = aabb.min.y;

            Vector3f normal = Vector3f(0.0, 1.0, 0.0);
            if (lowy < ground)
            {
                float penetration = ground - lowy;
                tf.position.y() += penetration;
                Vector3f hitPoint = Vector3f::ZERO;
                int contacts = 0;
                for (int i = 0; i < 8; i++)
                {
                    if (corners[i].y() < ground+0.01f)
                    {
                        hitPoint += corners[i];
                        contacts++;
                    }
                }
                hitPoint /= contacts;

                Vector3f r = hitPoint - tf.position;
                Vector3f rV = rb.velocity + (rb.angularVelocity ^ r);
                float nrV = rV * normal;
                bool isStaticLower =fabsf(rb.velocity.y())<0.1;
                if (nrV < -0.01f)
                {
                    float invMass = 1.0f / rb.mass;
                    float e = isStaticLower?0.0f:rb.elasticity * e_ground;
                    float i = -(1.0 + e) * nrV;
                    auto raxn = r ^ normal;
                    auto rot = tf.rotation.toMatrix();
                    auto worldInverseInertia = rot * rb.inverseInertiaTensor * rot.transposed();
                    float term = raxn * (worldInverseInertia * raxn);
                    float j = i / (term + invMass);
                    auto impulse = j * normal;
                    rb.AddImpulse(impulse, r);

                    // 摩擦力冲量
                    Vector3f tangent = rV - (normal * nrV);
                    if (tangent.LengthSquared() > 0.01f)
                    {
                        tangent.Normalize();
                        float vt = rV * tangent;
                        Vector3f raxt = r ^ tangent;
                        float angularTermT = raxt * (worldInverseInertia * raxt);
                        float jt = -vt / (invMass + angularTermT);
                        if (fabsf(jt) > j * mu)
                            jt = -j * mu;
                        Vector3f impulseT = jt * tangent;
                        rb.AddImpulse(impulseT, r);
                    }
                }

                // 防止不稳定
                if(isStaticLower&&fabsf(penetration)<=0.001f)
                {
                    rb.velocity.y() = 0.0f;
                    rb.AddForce(-m_gravity * rb.mass);

                    if(contacts>=3)
                    {
                        rb.angularMomentum *= friction;
                        rb.velocity *= friction;
                        if(rb.velocity.LengthSquared()<0.01f)
                            rb.velocity = Vector3f::ZERO;
                    }

                }
            }
        }
    }
}
