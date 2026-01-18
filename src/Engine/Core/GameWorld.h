#pragma once
#include "GameObject.h"
#include <vector>
#include <memory>

class GameWorld {
public:
    GameWorld() = default;
    ~GameWorld() = default;

    GameObject& CreateGameObject();
    void Update(float deltaTime);

    const std::vector<std::unique_ptr<GameObject>>& GetGameObjects() const;
    void DestroyWaitingObjects();
private:
    std::vector<std::unique_ptr<GameObject>> m_gameObjects;
};