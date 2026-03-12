#if !defined(PLATFORM_WEB)
#include "AIEnvironment.h"
#include "Engine/Core/Components/Components.h"
#include "rlgl.h"
#include <random>

AIEnvironment::AIEnvironment(GameWorld *gameWorld)
{
    m_gameWorld = gameWorld;
    Init();
}
AIEnvironment::~AIEnvironment()
{
    if (m_aiFbo.id > 0)
        UnloadRenderTexture(m_aiFbo);
}

void AIEnvironment::Init()
{
    m_aiFbo = Renderer::LoadRT(width, height, PIXELFORMAT_UNCOMPRESSED_R32G32B32A32);
    SetTextureFilter(m_aiFbo.texture, TEXTURE_FILTER_POINT);
    SetTextureFilter(m_aiFbo.depth, TEXTURE_FILTER_POINT);
}

RenderTexture2D &AIEnvironment::GetFbo()
{
    return m_aiFbo;
}
StepResult AIEnvironment::Reset()
{
    m_currentTime = 0.0f;
    m_gameWorld->Reset();
    auto *player = m_gameWorld->GetEntitiesByTag("Player")[0];
    auto *target = m_gameWorld->GetEntitiesByTag("Enemy")[0];

    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<float> dis(0.0f, 1.0f);
    player->GetComponent<TransformComponent>().SetWorldPosition(Vector3f(dis(gen) * 500 + 500, dis(gen) * 500 + 500, dis(gen) * 500 + 500));
    target->GetComponent<TransformComponent>().SetWorldPosition(Vector3f(dis(gen) * 500 + 500, dis(gen) * 500 + 500, dis(gen) * 500 + 500));

    player->GetComponent<RigidbodyComponent>().velocity = Vector3f(dis(gen) * 100, dis(gen) * 100, dis(gen) * 100);
    m_gameWorld->UpdateTransforms();
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

    m_currentTime += m_dt;
    m_gameWorld->FixedUpdate(m_dt);
    m_gameWorld->Update(m_dt);

    render.RenderAIView("AIView", *m_gameWorld, m_aiFbo);

    StepResult out;
    out.image_data = render.CaptureFrame(m_aiFbo);
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
std::vector<float> AIEnvironment::CaptureRGBD()
{
    std::vector<float> data(width * height * 4);
    std::vector<uint8_t> rgb(width * height * 4); // RGBA
    std::vector<uint8_t> depth(width * height);

    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_aiFbo.id);

    glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, rgb.data());
    glReadPixels(0, 0, width, height, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, depth.data());

    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

    for (int i = 0; i < width * height; i++)
    {
        data[i * 4 + 0] = rgb[i * 4 + 0] / 255.0f;
        data[i * 4 + 1] = rgb[i * 4 + 1] / 255.0f;
        data[i * 4 + 2] = rgb[i * 4 + 2] / 255.0f;
        data[i * 4 + 3] = depth[i];
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
    Vector3f toEnemy = (enemy->GetComponent<TransformComponent>().GetWorldPosition() - pTf.GetWorldPosition()).Normalized();
    float alignment = pTf.GetForward() * toEnemy;
    return alignment - 0.01f;
}

#endif