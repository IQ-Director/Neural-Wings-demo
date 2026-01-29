#pragma once
#include "Engine/Core/GameObject/GameObject.h"
#include "Engine/Graphics/Renderer.h"
#include "Engine/System/System.h"
#include <vector>
#include <memory>
#include <functional>
#include <string>
class GameWorld
{
public:
    GameWorld(std::function<void(Renderer &, PhysicsStageFactory &)> configCallback,
              const std::string &cameraConfigPath = "assets/config/cameras_config.json",
              const std::string &sceneConfigPath = "assets/scenes/test_scene.json",
              const std::string &inputConfigPath = "assets/config/input_config.json");
    ~GameWorld() = default;

    GameObject &CreateGameObject();
    void FixedUpdate(float fexedDeltaTime);
    bool Update(float deltaTime); // 返回true表示游戏继续，返回false表示游戏结束
    void Render();
    const std::vector<std::unique_ptr<GameObject>> &GetGameObjects() const;

    PhysicsStageFactory &GetPhysicsStageFactory() { return *m_physicsStageFactory; };
    PhysicsSystem &GetPhysicsSystem() { return *m_physicsSystem; };

    ResourceManager &GetResourceManager() { return *m_resourceManager; };

private:
    void DestroyWaitingObjects();
    std::vector<std::unique_ptr<GameObject>> m_gameObjects;

    std::unique_ptr<Renderer> m_renderer;
    std::unique_ptr<CameraManager> m_cameraManager;
    std::unique_ptr<InputManager> m_inputManager;

    std::unique_ptr<PhysicsStageFactory> m_physicsStageFactory;
    std::unique_ptr<PhysicsSystem> m_physicsSystem;

    std::unique_ptr<SceneManager> m_sceneManager;

    std::unique_ptr<ResourceManager> m_resourceManager;
};