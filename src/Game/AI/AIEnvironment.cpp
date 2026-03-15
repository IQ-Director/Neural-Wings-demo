#if !defined(PLATFORM_WEB)
#include "AIEnvironment.h"
#include "Engine/Core/Components/Components.h"

#include "Game/Systems/Physics/SolarStage.h"
#include "Game/Systems/Physics/NetworkVerifyStage.h"
#include "Game/Systems/Particles/Initializers/RandomLife.h"
#include "Game/Systems/Particles/Initializers/SphereDir.h"
#include "Game/Systems/Particles/Initializers/CollisionInit.h"
#include "Game/Systems/Particles/Initializers/SPHInit.h"
#include "Game/Systems/Particles/Initializers/ExplosionInit.h"

#include "Game/Scripts/Scripts.h"

#include "Game/Events/CombatEvents.h"

#include "rlgl.h"
#include <random>
#include <fstream>
#include <stdio.h>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

void AIEnvironment::initContext(int width, int height)
{
    __SHOWINFO__ = false;
    SetTraceLogLevel(LOG_NONE);
    if (!IsWindowReady())
    {
        SetConfigFlags(FLAG_WINDOW_HIDDEN);
        InitWindow(width, height, "AI Context");
        InitAudioDevice();
        SetTargetFPS(0);
        printf("[C++] GPU Context Initialized via Python\n");
    }
}
AIEnvironment::AIEnvironment(int width, int height)
{
    this->width = width;
    this->height = height;
    const std::string &cameraConfigPath = "assets/config/cameras_config.json";
    const std::string &sceneConfigPath = "assets/scenes/test_scene.json";
    const std::string &inputConfigPath = "assets/config/input_config.json";
    const std::string &renderViewConfigPath = "assets/view/test_view.json";
    const std::string &effectLibPath = "assets/Library/particle_effects.json";

    resourceManager = std::make_unique<ResourceManager>();
    audioManager = std::make_unique<AudioManager>(*resourceManager);

    // audioManager->LoadLibrary(audioPath);

    m_gameWorld = std::make_unique<GameWorld>([this](ScriptingFactory &scriptingFactory, PhysicsStageFactory &physicsStageFactory, ParticleFactory &particleFactory)
                                              { this->ConfigCallback(scriptingFactory, physicsStageFactory, particleFactory); },
                                              resourceManager.get(),
                                              audioManager.get(),
                                              cameraConfigPath,
                                              sceneConfigPath,
                                              inputConfigPath,
                                              renderViewConfigPath,
                                              effectLibPath);
    std::ifstream file(sceneConfigPath);
    if (!file.is_open())
    {
        std::cerr << "[AIEnvironment]: Failed to open scene file: " << sceneConfigPath << std::endl;
        return;
    }
    json sceneData = json::parse(file);

    m_aiFbo = Renderer::LoadRT(width, height, PIXELFORMAT_UNCOMPRESSED_R32G32B32A32);
    SetTextureFilter(m_aiFbo.texture, TEXTURE_FILTER_POINT);
    SetTextureFilter(m_aiFbo.depth, TEXTURE_FILTER_POINT);

    Init();
}
AIEnvironment::~AIEnvironment()
{
    if (m_aiFbo.id > 0)
        UnloadRenderTexture(m_aiFbo);
}
void AIEnvironment::ConfigCallback(ScriptingFactory &scriptingFactory, PhysicsStageFactory &physicsStageFactory, ParticleFactory &particleFactory)
{
    // 注册后才可使用json配置
    physicsStageFactory.Register("SolarStage", []()
                                 { return std::make_unique<SolarStage>(); });
    // physicsStageFactory.Register("NetworkVerifyStage", []()
    //                              { return std::make_unique<NetworkVerifyStage>(); });
    physicsStageFactory.Register("CollisionStage", []()
                                 { return std::make_unique<CollisionStage>(); });
    physicsStageFactory.Register("GravityStage", []()
                                 { return std::make_unique<GravityStage>(); });

    // 注册脚本
    scriptingFactory.Register("RotatorScript", []()
                              { return std::make_unique<RotatorScript>(); });
    scriptingFactory.Register("CollisionListener", []()
                              { return std::make_unique<CollisionListener>(); });
    scriptingFactory.Register("WeaponScript", []()
                              { return std::make_unique<WeaponScript>(); });
    scriptingFactory.Register("BulletScript", []()
                              { return std::make_unique<BulletScript>(); });
    scriptingFactory.Register("TrackingBulletScript", []()
                              { return std::make_unique<TrackingBulletScript>(); });
    scriptingFactory.Register("MineScript", []()
                              { return std::make_unique<MineScript>(); });

    scriptingFactory.Register("RayScript", []()
                              { return std::make_unique<RayScript>(); });
    scriptingFactory.Register("LocalPlayerSyncScript", []()
                              { return std::make_unique<LocalPlayerSyncScript>(); });
    scriptingFactory.Register("AudioScript", []()
                              { return std::make_unique<AudioScript>(); });
    scriptingFactory.Register("PlayerControlScript", []()
                              { return std::make_unique<PlayerControlScript>(); });
    scriptingFactory.Register("HealthScript", []()
                              { return std::make_unique<HealthScript>(); });

    // 注册粒子初始化器
    particleFactory.Register("SphereDir", []()
                             { return std::make_unique<SphereDir>(); });
    // particleFactory.Register("RadialVelocity", []()
    //                          { return std::make_unique<RadialVelocity>(); });
    particleFactory.Register("RandomLife", []()
                             { return std::make_unique<RandomLife>(); });
    particleFactory.Register("CollisionInit", []()
                             { return std::make_unique<CollisionInit>(); });
    particleFactory.Register("SPHInit", []()
                             { return std::make_unique<SPHInit>(); });
    particleFactory.Register("ExplosionInit", []()
                             { return std::make_unique<ExplosionInit>(); });
}

void AIEnvironment::Init()
{

    m_gameWorld->GetEventManager().Subscribe<CollisionEvent>([this](const CollisionEvent &e)
                                                             {
                                                                
        if (__SHOWINFO__)
std::cout << "CollisionEvent, impluse: " << e.impulse << std::endl;
//  e.hitpoint.print();
//  std::cout << "relative velocity: " << e.relativeVelocity.Length() << std::endl;
if (std::fabsf(e.relativeVelocity.Length()) < 2.0f || std::fabsf(e.impulse) < 10.0f)
return;

if (e.m_object2->GetTag() == "bullet" && e.m_object1->GetScript<HealthScript>())
{
    m_gameWorld->GetEventManager().Emit(DamageEvent(e.m_object1, 10.0f, e.hitpoint));
e.m_object2->SetIsWaitingDestroy(true);
}
if (e.m_object1->GetTag() == "bullet" && e.m_object2->GetScript<HealthScript>())
{
    m_gameWorld->GetEventManager().Emit(DamageEvent(e.m_object2, 10.0f, e.hitpoint));
e.m_object1->SetIsWaitingDestroy(true);
} });
}

RenderTexture2D &AIEnvironment::GetFbo()
{
    return m_aiFbo;
}
StepResult AIEnvironment::Reset()
{
    m_currentTime = 0.0f;
    m_gameWorld->Reset();
    // auto *player = m_gameWorld->GetEntitiesByTag("player")[0];
    // std::vector<GameObject *> target = m_gameWorld->GetEntitiesByTag("enemy");
    // if (!target.empty())
    // {
    //     auto *target = m_gameWorld->GetEntitiesByTag("enemy")[0];

    //     static std::random_device rd;
    //     static std::mt19937 gen(rd());
    //     static std::uniform_real_distribution<float> dis(0.0f, 1.0f);
    //     player->GetComponent<TransformComponent>().SetWorldPosition(Vector3f(dis(gen) * 500 + 500, dis(gen) * 500 + 500, dis(gen) * 500 + 500));
    //     target->GetComponent<TransformComponent>().SetWorldPosition(Vector3f(dis(gen) * 500 + 500, dis(gen) * 500 + 500, dis(gen) * 500 + 500));

    //     player->GetComponent<RigidbodyComponent>().velocity = Vector3f(dis(gen) * 100, dis(gen) * 100, dis(gen) * 100);
    //     m_gameWorld->UpdateTransforms();
    // }
    return Step({0, 0, 0, 0, 0, 0});
}
StepResult AIEnvironment::Step(const std::vector<float> &actions)
{
    auto &input = m_gameWorld->GetInputManager();
    auto &render = m_gameWorld->GetRenderer();

    input.SetAxisValue("Pitch", actions[0]);
    input.SetAxisValue("Yaw", actions[1]);
    input.SetAxisValue("Roll", actions[2]);

    input.SetKeyState("LEFT_SHIFT", actions[3] > 0.5f ? 1.0f : 0.0f);
    input.SetKeyState("LEFT_CONTROL", actions[4] > 0.5f ? 1.0f : 0.0f);
    input.SetKeyState("MOUSE_LEFT_BUTTON", actions[5] > 0.5f ? 1.0f : 0.0f);
    input.Update();

    m_currentTime += m_dt;

    m_gameWorld->FixedUpdate(m_dt);
    m_gameWorld->Update(m_dt);

    render.RenderAIView("AIView", *m_gameWorld, m_aiFbo);

    StepResult out;
    out.image_data = CaptureRGBD("AIView");
    out.reward = CalculateReward();
    out.done = IsDone();
    return out;
}
bool AIEnvironment::IsDone()
{
    auto *player = m_gameWorld->GetEntitiesByTag("player")[0];
    return (m_currentTime > 60.0f) || (!player || !player->IsActive());
}

#if defined(PLATFORM_WEB)
#include <GLES3/gl3.h>
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#else
#include "external/glad.h"
#endif
std::vector<float> AIEnvironment::CaptureRGBD(const std::string &cameraName)
{
    float near = m_gameWorld->GetCameraManager().GetCamera(cameraName)->getNearPlane();
    float far = m_gameWorld->GetCameraManager().GetCamera(cameraName)->getFarPlane();

    std::vector<float> data(width * height * 4);
    std::vector<uint8_t> rgb(width * height * 4); // RGBA
    std::vector<float> depth(width * height);

    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_aiFbo.id);

    glPixelStorei(GL_PACK_ALIGNMENT, 1);

    glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, rgb.data());
    glReadPixels(0, 0, width, height, GL_DEPTH_COMPONENT, GL_FLOAT, depth.data());

    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    for (int y = 0; y < height; y++)
        for (int x = 0; x < width; x++)
        {
            int source_idx = y * width + x;
            int tar_y = height - 1 - y;
            int i = tar_y * width + x;

            data[i * 4 + 0] = rgb[source_idx * 4 + 0] / 255.0f;
            data[i * 4 + 1] = rgb[source_idx * 4 + 1] / 255.0f;
            data[i * 4 + 2] = rgb[source_idx * 4 + 2] / 255.0f;

            float z_ndc = depth[source_idx] * 2.0f - 1.0f;
            float linearDepth = (2.0f * near * far) / (far + near - z_ndc * (far - near));

            data[i * 4 + 3] = linearDepth;
        }
    return data;
}

float AIEnvironment::CalculateReward()
{
    auto *player = m_gameWorld->GetEntitiesByTag("player")[0];
    std::vector<GameObject *> enemys = m_gameWorld->GetEntitiesByTag("enemy");
    if (!player || enemys.empty())
        return 0.0f;
    auto *enemy = enemys[0];
    auto &pTf = player->GetComponent<TransformComponent>();
    auto &eTf = enemy->GetComponent<TransformComponent>();

    Vector3f playerPos = pTf.GetWorldPosition();
    Vector3f enemyPos = eTf.GetWorldPosition();
    Vector3f followPoint = enemyPos - eTf.GetForward() * 20.0f;
    Vector3f diff = followPoint - playerPos;

    float distance = diff.Length();
    Vector3f toEnemy = diff.Normalized();

    float alignment = pTf.GetForward() * toEnemy;
    float distReward = std::exp(-0.002f * distance);

    float reward = 0.0f;

    if (alignment > 0)
        reward = alignment * distReward;
    else
        reward = alignment * 0.2f;

    if (alignment > 0.95f && distance < 200.0f)
    {
        reward += 0.2f;
        if (distance < 50.0f)
            reward += 0.3f;
    }
    reward -= 0.001f;
    return reward;
}
#endif