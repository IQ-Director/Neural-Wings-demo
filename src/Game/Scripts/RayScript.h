#pragma once
#include "Engine/Core/Components/Components.h"
#include <nlohmann/json.hpp>
using json = nlohmann::json;
class RayScript : public IScriptableComponent
{
public:
    float m_timer = 0.0f;
    float m_fireTimer = 0.0f;
    RayScript() = default;
    void OnFixedUpdate(float fixedDeltaTime) override;
};
