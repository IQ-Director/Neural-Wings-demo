#include "GameWorld.h"
#include <algorithm>
GameObject& GameWorld::CreateGameObject() {
    auto newObject = std::make_unique<GameObject>();
    GameObject* rawPtr = newObject.get();
    m_gameObjects.push_back(std::move(newObject));
    return *rawPtr;
}

void GameWorld::Update(float deltaTime) {
    // TODO: 更新世界中的所有 GameObject
    // ScriptingSystem->Update(), PhysicsSystem->Update() 等。
}

const std::vector<std::unique_ptr<GameObject>>& GameWorld::GetGameObjects() const {
    return m_gameObjects;
}

void GameWorld::DestroyWaitingObjects() {
    m_gameObjects.erase(
        std::remove_if(
            m_gameObjects.begin(), 
            m_gameObjects.end(),
            [](const std::unique_ptr<GameObject>& object) {
                return object->IsWaitingDestroy();
            }
        ),
        m_gameObjects.end()
    );
}
