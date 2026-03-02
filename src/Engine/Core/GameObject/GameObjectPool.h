#pragma once
#include <vector>
#include <string>
#include "GameObject.h"

class GameWorld;

class GameObjectPool
{
public:
    GameObjectPool(std::string preafab_path, GameWorld &world);
    ~GameObjectPool();

    void Preload(size_t count);
    GameObject *Spawn(const std::string &name, const Vector3f &position, const Quat4f &rotation);
    void Recycle(GameObject *obj);

    const std::string &GetPrefabPath() const { return m_prefab_path; }

private:
    std::string m_prefab_path;
    GameWorld &m_world;

    // InAvtive Pool
    std::vector<GameObject *> m_pool;
};