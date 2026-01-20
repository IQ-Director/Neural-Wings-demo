#pragma once
#include "GameScreen.h"
#include "ScreenFactory.h"
#include "Engine/Config/EngineConfig.h"
#include "Engine/System/Time/TimeManager.h"
#include <memory>

class ScreenManager {
public:
    ScreenManager(const EngineConfig& config, std::unique_ptr<ScreenFactory> factory);
    ~ScreenManager();
    //void Run();
    
    void ApplySettings(const EngineConfig& config);

    bool UpdateFrame(); 
    void Shutdown();
private:
    void ChangeScreen(int newState);

    std::unique_ptr<GameScreen> m_currentScreen;
    std::unique_ptr<ScreenFactory> m_factory;

    TimeManager m_timeManager;
    float m_accumulator;
};