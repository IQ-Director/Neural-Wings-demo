#pragma once

class TimeManager
{
public:
    TimeManager(float targetFPS = 60.0f);
    void Tick();
    void TickGame(float fixedDeltaTime);
    void setFPS(float targetFPS);
    // 获取可变的 delta time (用于渲染和非物理逻辑)
    float GetDeltaTime() const;

    // 获取固定的 delta time (用于物理和网络)
    float GetFixedDeltaTime() const;

    float GetGameTime() const;
    float GetRealTime() const;

private:
    double m_lastFrameTime;
    float m_deltaTime;
    float m_fixedDeltaTime;

    float m_currentRealTime; // 以启动为起点的当前现实时间

    float m_currentGameTime; // 以启动为起点的当前游戏时间
    float m_deltaGameTime;
    float m_lastGameTime;
};