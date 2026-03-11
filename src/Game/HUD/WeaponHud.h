#pragma once
#include "Engine/System/HUD/IGameHud.h"
#include "raylib.h"

class GameWorld;
class GameObject;

class WeaponHud : public IGameHud
{
public:
    explicit WeaponHud(GameWorld *world);
    ~WeaponHud() override = default;

    void OnEnter() override;
    void FixedUpdate(float fixedDeltaTime) override {}
    void Update(float deltaTime) override;
    void Draw() override;
    void OnExit() override {}

private:
    GameWorld *m_world = nullptr;
    GameObject *GetLocalPlayer() const;

    int m_lastWeaponType = -1;
    float m_displayTimer = 0.0f;
    float m_alpha = 0.0f;

    const float m_maxDisplayTime = 2.5f;
    const float m_fadeTime = 0.1f;
};