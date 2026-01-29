#include "GameplayScreen.h"
#include "raylib.h"
#include "Game/Screen/MyScreenState.h"
#include "Game/Systems/Physics/SolarStage.h"
#include "raymath.h"
#include "Engine/Engine.h"
#include <iostream>
GameplayScreen::GameplayScreen()
    : m_nextScreenState(SCREEN_STATE_NONE)
{
    m_world = std::make_unique<GameWorld>([this](Renderer &renderer, PhysicsStageFactory &factory)
                                          { this->ConfigCallback(renderer, factory); });
}
GameplayScreen::~GameplayScreen()
{
    OnExit();
}

void GameplayScreen::ConfigCallback(Renderer &renderer, PhysicsStageFactory &factory)
{
    ConfigureRenderer(renderer);
    // 注册后才可使用json配置
    factory.Register("solarStage", []()
                     { return std::make_unique<SolarStage>(); });
    factory.Register("collisionStage", []()
                     { return std::make_unique<CollisionStage>(); });
}

void GameplayScreen::ConfigureRenderer(Renderer &renderer)
{
    renderer.ClearRenderViews();

    // 1. 主视图
    RenderView mainView;
    mainView.cameraName = "main_orbital";
    mainView.viewport = {0, 0, (float)GetScreenWidth(), (float)GetScreenHeight()};
    renderer.AddRenderView(mainView);

    // 2.后视图
    RenderView rearView;
    rearView.cameraName = "rear_view";
    int viewWidth = GetScreenWidth() / 5;
    int viewHeight = GetScreenHeight() / 5;
    rearView.viewport = {(float)GetScreenWidth() - viewWidth - 20, (float)GetScreenHeight() - viewHeight - 20, (float)viewWidth, (float)viewHeight};
    rearView.clearBackground = true; // 不透明底
    rearView.backgroundColor = Fade(BLACK, 0.7f);
    renderer.AddRenderView(rearView);
}

// 当进入游戏场景时调用
void GameplayScreen::OnEnter()
{
    DisableCursor();
}

// 当离开游戏场景时调用
void GameplayScreen::OnExit()
{
    EnableCursor();
    // m_sceneManager->UnloadCurrentScene();
}
// TODO:主循环

// 在固定时间步更新（未来的物理和网络逻辑将在这里）
void GameplayScreen::FixedUpdate(float fixedDeltaTime)
{
    // m_networkManager->Update(fixedDeltaTime);
    // auto *mainCam = m_cameraManager->GetMainCamera();
    // Vector3 mainPos = mainCam->position;
    // mainPos = Vector3Add(mainPos, Vector3Scale(mainCam->direction, 0.2f));

    // mainCam->UpdateFromDirection(mainPos, mainCam->direction, mainCam->up);

    m_world->FixedUpdate(fixedDeltaTime);
}

void GameplayScreen::Update(float deltaTime)
{
    m_nextScreenState = SCREEN_STATE_NONE;
    if (!m_world->Update(deltaTime))
        m_nextScreenState = MAIN_MENU;
}

void GameplayScreen::Draw()
{
    ClearBackground(RAYWHITE); // 设置一个浅灰色背景

    // m_renderer->RenderScene(*m_world, *m_cameraManager);
    m_world->Render();
    // 在3D内容之上绘制一些2D的调试信息
    DrawText("Welcome to the 3D World!", 10, 40, 20, DARKGRAY);
    DrawText("Press ESC to return.", 10, GetScreenHeight() - 30, 20, DARKGRAY);
    // m_worldRenderer->Draw(*m_world, *m_cameraManager);
    // m_uiManager->Draw(*m_world, *m_cameraManager);
}

// 向 ScreenManager 报告下一个状态
ScreenState GameplayScreen::GetNextScreenState() const
{
    return m_nextScreenState;
}
ScreenState GameplayScreen::GetScreenState() const
{
    return GAMEPLAY;
}