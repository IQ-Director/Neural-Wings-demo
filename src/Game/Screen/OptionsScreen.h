#pragma once
#include "Engine/System/Screen/GameScreen.h"
#include "Engine/Config/EngineConfig.h"
#include "ScreenState.h"

class OptionsScreen : public GameScreen
{
public:
    OptionsScreen();
    ~OptionsScreen();

    void OnEnter() override;
    void FixedUpdate(float fixedDeltaTime) override;
    void Update(float deltaTime) override;
    void Draw() override;
    void OnExit() override;
    int GetNextScreenState() const override;
    int GetScreenState() const override;

private:
    int m_nextScreenState;

    EngineConfig m_currentConfig;  // 原始配置
    EngineConfig m_modifiedConfig; // 修改后配置
    bool m_fullscreenToggle;

    bool m_resolutionDropdown;
    int m_resolutionDropdownActive; // 0: 1280x720, 1: 1600x900, 2: 1920x1080, 3: 2560x1440
    const char* m_resolutionOptionsText = "1280x720;1600x900;1920x1080;2560x1440";
    static constexpr int RESOLUTION_WIDTHS[4] = {1280, 1600, 1920, 2560};
    static constexpr int RESOLUTION_HEIGHTS[4] = {720, 900, 1080, 1440};

    void SaveConfig();
};
