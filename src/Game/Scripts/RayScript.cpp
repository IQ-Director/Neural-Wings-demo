#include "RayScript.h"
#include "Engine/Core/GameObject/GameObject.h"
#include "Engine/Core/GameWorld.h"
#include "Engine/System/Ray/mRay.h"
void RayScript::OnFixedUpdate(float fixedDeltaTime)
{
    auto &input = owner->GetOwnerWorld()->GetInputManager();
    if (input.IsActionDown("Ray"))
    {
        m_timer += fixedDeltaTime;
        while (m_timer >= 0.05f)
        {
            m_timer -= 0.05f;
            auto &tf = owner->GetComponent<TransformComponent>();
            mRay ray(tf.GetWorldPosition() + tf.GetForward() * 2.0f, tf.GetForward());
            mRaycastHit hit = ray.Raycast(1000.0f, *owner->GetOwnerWorld(), owner);
            if (hit.hit)
            {

                if (__SHOWINFO__)
                    std::cout << "Ray hit: " << (hit.entity ? hit.entity->GetName() : "Unknown") << " at distance: " << hit.distance << std::endl;
            }
        }
    }
    else
    {
        m_timer = 0.049f;
    }
}