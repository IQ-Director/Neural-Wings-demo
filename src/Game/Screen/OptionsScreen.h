#pragma once
#include "Engine/System/Screen/IGameScreen.h"
#include "Engine/Config/EngineConfig.h"
#include "Engine/Network/Client/NetworkClient.h"
#include "MyScreenState.h"

class OptionsScreen : public IGameScreen
{
public:
    OptionsScreen(ScreenManager *sm);
    ~OptionsScreen();

    void OnEnter() override;
    void FixedUpdate(float fixedDeltaTime) override;
    void Update(float deltaTime) override;
    void Draw() override;
    void OnExit() override;
    ScreenState GetNextScreenState() const override;
    ScreenState GetScreenState() const override;

private:
    ScreenState m_nextScreenState;
    bool m_pendingSync = false;

    EngineConfig m_currentConfig;  // 原始配置
    EngineConfig m_modifiedConfig; // 修改后配置
    void SaveConfig();
    void ApplyVueSettings(); // Apply settings from Vue to m_modifiedConfig
    void ApplyConfigToUI();

    // ── Server check bridge (single global NetworkClient only) ─────
    void UpdatePingCheck(float deltaTime);
    bool m_waitingServerCheck = false;
    float m_serverCheckTimer = 0.0f;
    static constexpr float SERVER_CHECK_TIMEOUT = 5.0f;

    // ── Nickname bridge ────────────────────────────────────────────
    void HandleNicknameApplyRequest();
    void StartNicknameFetch();
    void UpdateNicknameFetch(float deltaTime);
    bool m_waitingNicknameFetch = false;
    float m_nicknameFetchTimer = 0.0f;
    static constexpr float NICKNAME_FETCH_TIMEOUT = 5.0f;
};
