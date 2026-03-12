#pragma once
#if !defined(PLATFORM_WEB)
#include <vector>
#include <string>
#include <memory>
#include "Engine/Core/GameWorld.h"
#include "Engine/Graphics/Renderer.h"

struct StepResult
{
    std::vector<float> image_data;
    std::vector<float> sensors;
    float reward;
    bool done;
};

class AIEnvironment
{
public:
    AIEnvironment(GameWorld *gameWorld);
    ~AIEnvironment();
    void Init();

    StepResult Reset();
    StepResult Step(const std::vector<float> &action);

    RenderTexture2D &GetFbo();

private:
    GameWorld *m_gameWorld;

    RenderTexture2D m_aiFbo;
    int width = 64;
    int height = 64;

    float m_currentTime = 0.0f;
    float m_dt = 1.0f / 60.0f;

    float CalculateReward();
    std::vector<float> CaptureRGBD();
    bool IsDone();
};
#endif