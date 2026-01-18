#include "TimeManager.h"
#include "raylib.h" 

TimeManager::TimeManager(float targetFPS)
    : m_lastFrameTime(GetTime()),
      m_deltaTime(0.0f),
      m_fixedDeltaTime(1.0f / targetFPS) { // 固定每秒更新 60 次
}

void TimeManager::Tick() {
    double currentTime = GetTime();
    m_deltaTime = static_cast<float>(currentTime - m_lastFrameTime);
    m_lastFrameTime = currentTime;
}
void TimeManager::setFPS(float targetFPS) {
    m_fixedDeltaTime = (1.0f / targetFPS);
}
float TimeManager::GetDeltaTime() const {
    return m_deltaTime;
}

float TimeManager::GetFixedDeltaTime() const {
    return m_fixedDeltaTime;
}