#include "GameWorld.h"
#include <algorithm>
#include "Engine/System/System.h"
#include "Engine/Graphics/Graphics.h"
#include <string>

GameWorld::GameWorld(std::function<void(Renderer &, PhysicsStageFactory &)> configCallback,
                     const std::string &cameraConfigPath,
                     const std::string &sceneConfigPath,
                     const std::string &inputConfigPath)
{

    m_renderer = std::make_unique<Renderer>();
    m_cameraManager = std::make_unique<CameraManager>();
    m_inputManager = std::make_unique<InputManager>();
    m_physicsSystem = std::make_unique<PhysicsSystem>();
    m_physicsStageFactory = std::make_unique<PhysicsStageFactory>();
    m_resourceManager = std::make_unique<ResourceManager>();
    configCallback(*m_renderer, *m_physicsStageFactory);

    m_cameraManager->LoadConfig(cameraConfigPath);
    m_sceneManager->LoadScene(sceneConfigPath, *this, *m_physicsSystem);

    if (!m_inputManager->LoadBindings(inputConfigPath))
    {
        std::cerr << "Error: [GameplayScreen] Could not load input bindings.Use default bindings instead." << std::endl;
        m_inputManager->LoadBindings("assets/config/default/input_config.json");
    }
}
GameObject &GameWorld::CreateGameObject()
{
    auto newObject = std::make_unique<GameObject>();
    GameObject *rawPtr = newObject.get();
    m_gameObjects.push_back(std::move(newObject));
    return *rawPtr;
}

void GameWorld::FixedUpdate(float fixedDeltaTime)
{
    // TODO: 更新世界中的所有 GameObject
    // ScriptingSystem->Update(), PhysicsSystem->Update() 等。
    m_physicsSystem->Update(*this, fixedDeltaTime);
    this->DestroyWaitingObjects();
}
bool GameWorld::Update(float deltaTime)
{
    m_inputManager->Update();
    if (m_inputManager->IsActionPressed("Exit"))
    {
        return false;
    }

    // TODO:Camera更新
    if (auto *mainCam = m_cameraManager->GetMainCamera())
    {
        Vector3f mainPos = mainCam->Position();

        if (m_inputManager->IsActionDown("Forward"))
        {
            DrawText("Forward!", 200, 200, 20, GREEN);
            mainPos += mainCam->Direction() * 0.3f;
        }
        if (m_inputManager->IsActionDown("Backward"))
        {
            DrawText("Backward!", 200, 200, 20, GREEN);
            mainPos -= mainCam->Direction() * 0.1f;
        }
        if (m_inputManager->IsActionDown("Left"))
        {
            DrawText("Left!", 200, 200, 20, GREEN);
            mainPos -= mainCam->Right() * 0.1f;
        }
        if (m_inputManager->IsActionDown("Right"))
        {
            DrawText("Right!", 200, 200, 20, GREEN);
            mainPos += mainCam->Right() * 0.1f;
        }
        mainCam->UpdateFromDirection(mainPos, mainCam->Direction(), mainCam->Up());

        float lookHorizontal = -m_inputManager->GetAxisValue("LookHorizontal") * PI / 180;
        float lookVertical = m_inputManager->GetAxisValue("LookVertical") * PI / 180;
        DrawText(TextFormat("LookHorizontal: %f", lookHorizontal), 200, 300, 20, GREEN);
        DrawText(TextFormat("LookVertical: %f", lookVertical), 200, 350, 20, GREEN);
        mainCam->Rotate(lookHorizontal, lookVertical);

        if (auto *rearCam = m_cameraManager->GetCamera("rear_view"))
        {
            Vector3f mainPos = mainCam->Position();
            Vector3f mainTarget = mainCam->Target();
            Vector3f direction = mainTarget - mainPos;
            direction.Normalize();

            rearCam->UpdateFromDirection(mainPos, -direction, mainCam->Up());
        }
    }
}

const std::vector<std::unique_ptr<GameObject>> &GameWorld::GetGameObjects() const
{
    return m_gameObjects;
}

void GameWorld::DestroyWaitingObjects()
{
    m_gameObjects.erase(
        std::remove_if(
            m_gameObjects.begin(),
            m_gameObjects.end(),
            [](const std::unique_ptr<GameObject> &object)
            {
                return object->IsWaitingDestroy();
            }),
        m_gameObjects.end());
}
void GameWorld::Render()
{
    m_renderer->RenderScene(*this, *m_cameraManager);
}