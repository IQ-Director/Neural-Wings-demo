#include "GameplayScreen.h"
#include "raylib.h"
#include "ScreenState.h"

GameplayScreen::GameplayScreen()
    : m_nextScreenState(static_cast<int>(ScreenStateID::NONE))
{
}

// 当进入游戏场景时调用
void GameplayScreen::OnEnter() {
    // m_world = std::make_unique<GameWorld>();
    // m_sceneManager->LoadScene("assets/scenes/earth_map.json");
}

// 当离开游戏场景时调用
void GameplayScreen::OnExit() {
    // m_sceneManager->UnloadCurrentScene();
}

// 在固定时间步更新（未来的物理和网络逻辑将在这里）
void GameplayScreen::FixedUpdate(float fixedDeltaTime) {
    // m_physicsSystem->Update(*m_world, fixedDeltaTime);
    // m_networkManager->Update(fixedDeltaTime);
}

void GameplayScreen::Update(float deltaTime) {
    m_nextScreenState = static_cast<int>(ScreenStateID::NONE);

    if (IsKeyPressed(KEY_ESCAPE)) {
        m_nextScreenState = static_cast<int>(ScreenStateID::MAIN_MENU);
    }
    
    // m_scriptingSystem->Update(*m_world, deltaTime);
    m_world->DestroyWaitingObjects();
}

void GameplayScreen::Draw() {
    ClearBackground(DARKGRAY);

    DrawText("GAMEPLAY ACTIVE", GetScreenWidth() / 2 - MeasureText("GAMEPLAY ACTIVE", 40) / 2, 200, 40, RAYWHITE);
    DrawText("This is where the magic happens!", 20, 60, 20, LIGHTGRAY);
    DrawText("Press [ESC] to return to Main Menu", 20, GetScreenHeight() - 40, 20, LIGHTGRAY);

    // m_worldRenderer->Draw(*m_world, *m_cameraManager);
    // m_uiManager->Draw(*m_world, *m_cameraManager);
}

// 向 ScreenManager 报告下一个状态
int GameplayScreen::GetNextScreenState() const {
    return m_nextScreenState;
}
int GameplayScreen::GetScreenState() const {
    return static_cast<int>(ScreenStateID::GAMEPlAY);
}