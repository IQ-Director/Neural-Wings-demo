#pragma once
#include "Engine/Core/Components/Components.h"
#include <nlohmann/json.hpp>
using json = nlohmann::json;
class GameObject;
class InputManager;

class BulletScript : public IScriptableComponent
{
public:
    float lifeTime = 2.0f;
    float timer = 0.0f;
    float m_fireTimer = 0.0f;
    BulletScript() = default;
    void OnFixedUpdate(float fixedDeltaTime) override;

    void Initialize(const json &data) override;
    void OnWake() override;
};

class TrackingBulletScript : public IScriptableComponent
{
public:
    void Initialize(const json &data) override;
    void OnWake() override;
    void OnFixedUpdate(float dt) override;
    void SetTarget(GameObject *target) { m_target = target; }

private:
    GameObject *m_target = nullptr;
    float m_thrust = 40.0f;
    float m_steerSensitivity = 25.0f;
    float m_timer = 0.0f;
    float m_lifeTime = 5.0f;
    float m_damping = 0.5f;
};

class WeaponScript : public IScriptableComponent
{
public:
    void Initialize(const json &data) override;
    WeaponScript() = default;
    void OnUpdate(float deltaTime) override;

    float m_fireTimer = 0.0f;

    float m_fireRate_0 = 0.15f;
    float m_bulletVelocity_0 = 0.0f;
    float m_fireRate_1 = 0.15f;
    float m_bulletVelocity_1 = 0.0f;

    int bulletType = 0;
    int bulletTypeCount = 2;

    void Bullet0(const InputManager &input);
    void Bullet1(const InputManager &input);
};