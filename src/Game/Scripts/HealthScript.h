#pragma once
#include "Engine/Core/Components/IScriptableComponent.h"
#include "Engine/Core/Events/EventManager.h"
#include "Engine/Math/Math.h"
#include <nlohmann/json.hpp>
using json = nlohmann::json;

class HealthScript : public IScriptableComponent
{
public:
    void Initialize(const json &data) override;
    void OnCreate() override;
    void OnWake() override;
    void OnDestroy() override;
    void OnUpdate(float dt) override;

private:
    Subscription_ID m_subID = 0;

    float maxHP = 100.0f;
    float currentHP = 100.0f;
    float m_hitFlashTimer = 0.0f;
    float m_flashDuration = 0.2f;
    Vector4f m_defaultColor = Vector4f(255.0f);
    Vector4f m_hitFlashColor = Vector4f(255.0f, 0.0f, 0.0f, 255.0f);
};