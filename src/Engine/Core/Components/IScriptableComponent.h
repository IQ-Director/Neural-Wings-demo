#pragma once
#include <nlohmann/json.hpp>

class GameObject;
class GameWorld;
class IScriptableComponent
{
public:
    virtual ~IScriptableComponent() = default;

    virtual void OnCreate() {};

    virtual void OnWake() {};  // 从池中取出时调用；
    virtual void OnSleep() {}; // 放回池中时调用；

    virtual void OnUpdate(float deltaTime) {};
    virtual void OnFixedUpdate(float fixedDeltaTime) {};
    virtual void OnDestroy() {};

    virtual void Initialize(const nlohmann::json &json) {};

    GameObject *owner = nullptr;
    GameWorld *world = nullptr;
};