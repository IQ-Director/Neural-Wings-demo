#pragma once

constexpr int SCREEN_STATE_MAIN_MENU = 0;
constexpr int SCREEN_STATE_START = -1;
constexpr int SCREEN_STATE_NONE = -2;
constexpr int SCREEN_STATE_EXIT = -3;
constexpr int SCREEN_STATE_ERROR = -4;

class ScreenManager;

class GameScreen
{
public:
    virtual ~GameScreen() = default;

    // 激活调用（初始化）
    virtual void OnEnter() = 0;

    // 固定时间步长调用（物理、网络）
    virtual void FixedUpdate(float fixedDeltaTime) = 0;

    // 每一帧都调用（输入、非物理逻辑）
    virtual void Update(float deltaTime) = 0;

    // Update 后（绘制）
    virtual void Draw() = 0;

    // 卸载资源、清理
    virtual void OnExit() = 0;

    // 状态
    virtual int GetNextScreenState() const = 0;
    virtual int GetScreenState() const = 0;

    ScreenManager* screenManager = nullptr;
};
