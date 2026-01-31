#include "TimeManager.h"
#include "raylib.h"

TimeManager::TimeManager(float targetFPS)
    : m_lastFrameTime(GetTime()),
      m_deltaTime(0.0f),
      m_fixedDeltaTime(1.0f / targetFPS),
      m_currentRealTime(0.0f),
      m_currentGameTime(0.0f),
      m_lastGameTime(0.0f),
      m_deltaGameTime(0.0f)
{ // 固定每秒更新 60 次
}

void TimeManager::Tick()
{
    double currentTime = GetTime();
    m_deltaTime = static_cast<float>(currentTime - m_lastFrameTime);

    m_lastFrameTime = currentTime;
    m_currentRealTime += m_deltaTime;
}
void TimeManager::TickGame(float fixedDeltaTime)
{
    m_lastGameTime = m_currentGameTime;
    m_currentGameTime += fixedDeltaTime;
}
void TimeManager::setFPS(float targetFPS)
{
    m_fixedDeltaTime = (1.0f / targetFPS);
    m_deltaGameTime = m_fixedDeltaTime;
}
float TimeManager::GetDeltaTime() const
{
    return m_deltaTime;
}

float TimeManager::GetFixedDeltaTime() const
{
    return m_fixedDeltaTime;
}

float TimeManager::GetGameTime() const
{
    return m_currentGameTime;
}
float TimeManager::GetRealTime() const
{
    return m_currentRealTime;
}