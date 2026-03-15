#pragma once
#if !defined(PLATFORM_WEB)
#include <vector>
#include <string>
#include <memory>
#include "Engine/Engine.h"

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
    static void initContext(int width, int height);
    AIEnvironment(int width, int height);
    AIEnvironment() = default;
    // AIEnvironment(GameWorld *gameWorld);
    ~AIEnvironment();
    void Init();

    StepResult Reset();
    StepResult Step(const std::vector<float> &action);

    RenderTexture2D &GetFbo();
    float GetTime() const { return m_currentTime; }
    int GetWidth() const { return width; }
    int GetHeight() const { return height; }

private:
    std::unique_ptr<GameWorld> m_gameWorld;
    std::unique_ptr<ResourceManager> resourceManager;
    std::unique_ptr<AudioManager> audioManager;

    RenderTexture2D m_aiFbo;

    int width = 64;
    int height = 64;

    float m_currentTime = 0.0f;
    float m_dt = 1.0f / 60.0f;

    float CalculateReward();
    std::vector<float> CaptureRGBD(const std::string &cameraName);
    bool IsDone();

    void ConfigCallback(ScriptingFactory &scriptingFactory,
                        PhysicsStageFactory &physicsStageFactory,
                        ParticleFactory &particleFactory);
};
#endif