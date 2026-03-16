#include "HealthScript.h"
#include "Engine/Core/GameObject/GameObject.h"
#include "Engine/Core/GameWorld.h"
#include "Engine/Core/Components/Components.h"
#include "Game/Events/CombatEvents.h"

void HealthScript::Initialize(const json &data)
{
    m_flashDuration = data.value("flashDuration", 0.2f);
    maxHP = data.value("maxHP", 100.0f);
    currentHP = maxHP;
}
void HealthScript::OnCreate()
{
    auto &em = owner->GetOwnerWorld()->GetEventManager();
    m_subID = em.Subscribe<DamageEvent>([this](const DamageEvent &e)
                                        {
                                            if (e.victim == this->owner && this->currentHP > 0.0f)
                                            {
                                                this->currentHP -= e.amount;
                                                this->m_hitFlashTimer = m_flashDuration;
                                                
                                                if (__SHOWINFO__)
                                                std::cout << "HP: " << this->currentHP << std::endl;
                                                // TODO: 粒子受伤特效
                                                // auto &renderer = owner->GetComponent<RenderComponent>();
                                                // renderer.totalBaseColor = Vector4f(255.0f, 0.0f, 0.0f, 1.0f);
                                                if (this->currentHP <= 0.0f)
                                                {
                                                    this->currentHP = 0.0f;
                                                    owner->GetOwnerWorld()->GetEventManager().Emit(DeathEvent(this->owner));
                                                    //TODO: 死亡爆炸效果
                                                    //TODO: 相机转移
                                                    owner->SetActive(false);
                                                }
                                            } });
}
void HealthScript::OnWake()
{
    currentHP = maxHP;
}
void HealthScript::OnDestroy()
{
    if (m_subID != 0)
        owner->GetOwnerWorld()->GetEventManager().Unsubscribe(m_subID);
}
void HealthScript::OnUpdate(float deltaTime)
{
    if (m_hitFlashTimer > 0.0f)
    {
        m_hitFlashTimer -= deltaTime;
        if (m_hitFlashTimer < 0.0f)
            m_hitFlashTimer = 0.0f;

        if (owner->HasComponent<RenderComponent>())
        {
            auto &rc = owner->GetComponent<RenderComponent>();

            float t = m_hitFlashTimer / m_flashDuration;
            rc.totalBaseColor = Vector4f::Lerp(m_defaultColor, m_hitFlashColor, t);
        }
    }
}