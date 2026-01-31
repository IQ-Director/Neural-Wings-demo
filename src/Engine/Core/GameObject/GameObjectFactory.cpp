#pragma once
#include "GameObjectFactory.h"
#include "Engine/Math/Math.h"
#include "Engine/Core/Components/Components.h"
#include "Engine/Utils/JsonParser.h"
#include "raylib.h"
#include <fstream>
#include <nlohmann/json.hpp>
#include <iostream>
using json = nlohmann::json;

GameObject &GameObjectFactory::CreateFromPrefab(const std::string &name, const std::string &tag, const std::string &path, GameWorld &world)
{
    std::ifstream file(path);
    json data = json::parse(file);
    file.close();

    GameObject &gameObject = world.CreateGameObject();
    gameObject.SetName(name);

    gameObject.SetTag(tag);

    if (data.contains("components"))
    {
        // 严格顺序，确保transform在rigid前面
        for (auto &comp : data["components"])
        {
            auto it = comp.begin();
            if (it != comp.end())
            {
                const std::string compName = it.key();
                const json &compData = it.value();
                ApplyComponent(world, gameObject, compName, compData);
            }
        }
    }
    return gameObject;
}

void GameObjectFactory::ApplyComponent(GameWorld &gameWorld, GameObject &gameObject, const std::string &compName, const json &prefab)
{
    if (compName == "TransformComponent")
    {
        ParseTransformComponent(gameObject, prefab);
    }
    else if (compName == "RenderComponent")
    {
        ParseRenderComponent(gameWorld, gameObject, prefab);
    }
    else if (compName == "RigidBodyComponent")
    {
        ParseRigidBodyComponent(gameObject, prefab);
    }
    else if (compName == "ScriptComponent")
    {
        ParseScriptComponent(gameWorld, gameObject, prefab);
    }
    else
        std::cerr << "Component " << compName << " not implemented" << std::endl;
}

void GameObjectFactory::ParseRenderComponent(GameWorld &gameWorld, GameObject &gameObject, const json &prefab)
{
    auto &rd = gameObject.AddComponent<RenderComponent>();
    auto &rm = gameWorld.GetResourceManager();
    rd.model = rm.GetModel(prefab.value("model", "primitive://cube"));
    if (prefab.contains("material"))
    {
        auto &matData = prefab["material"];
        if (matData.contains("vs") && matData.contains("fs"))
        {
            rd.material.shader = rm.GetShader(matData["vs"], matData["fs"]);
        }
        if (matData.contains("color"))
        {
            rd.material.baseColor = JsonParser::ToVector4f(matData["color"]);
        }
    }
    if (prefab.contains("scale"))
        rd.scale = JsonParser::ToVector3f(prefab["scale"]);
}

void GameObjectFactory::ParseTransformComponent(GameObject &gameObject, const json &prefab)
{
    auto &tf = gameObject.AddComponent<TransformComponent>();
    if (prefab.contains("position"))
        tf.position = JsonParser::ToVector3f(prefab["position"]);
    if (prefab.contains("scale"))
        tf.scale = JsonParser::ToVector3f(prefab["scale"]);
    if (prefab.contains("rotation"))
        tf.rotation = Quat4f(JsonParser::ToVector3f(prefab["rotation"]));
}
void GameObjectFactory::ParseRigidBodyComponent(GameObject &gameObject, const json &prefab)
{
    if (!gameObject.HasComponent<TransformComponent>())
    {
        std::cerr << "[GameObjectFactory]: RigidbodyComponent requires TransformComponent!!!" << std::endl;
        return;
    }
    auto &rb = gameObject.AddComponent<RigidbodyComponent>();
    auto &tf = gameObject.GetComponent<TransformComponent>();
    rb.mass = prefab.value("mass", 1.0f);
    rb.drag = prefab.value("drag", 0.0f);
    rb.angularDrag = prefab.value("angularDrag", 0.0f);
    rb.elasticity = prefab.value("elasticity", 0.0f);

    if (prefab.contains("velocity"))
        rb.velocity = JsonParser::ToVector3f(prefab["velocity"]);
    if (prefab.contains("angularVelocity"))
        rb.angularVelocity = JsonParser::ToVector3f(prefab["angularVelocity"]);

    std::string colliderType = prefab.value("colliderType", "BOX");
    if (colliderType == "BOX")
        rb.colliderType = ColliderType::BOX;
    else if (colliderType == "SPHERE")
        rb.colliderType = ColliderType::SPHERE;
    else
        std::cerr << "Unknown collider type: " << colliderType << std::endl;
    rb.SetHitbox(tf.scale);
}
void GameObjectFactory::ParseScriptComponent(GameWorld &gameWorld, GameObject &gameObject, const json &prefab)
{

    auto &sc = gameObject.AddComponent<ScriptComponent>();
    auto &factory = gameWorld.GetScriptingFactory();
    for (auto &[scriptName, scriptData] : prefab.items())
    {
        auto script = factory.Create(scriptName);
        if (script)
        {
            script->world = &gameWorld;
            script->owner = &gameObject;
            script->Initialize(scriptData);
            script->OnCreate();
            sc.scripts.push_back(std::move(script));
        }
    }
}