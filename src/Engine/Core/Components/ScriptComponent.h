#pragma once
#include "IComponent.h"
#include "IScriptableComponent.h"
#include <memory>
#include <vector>

struct ScriptComponent : public IComponent
{
    std::vector<std::unique_ptr<IScriptableComponent>> scripts;

    bool m_logicDestroyed = false;
    void ExcuteOnDestroy()
    {
        if (m_logicDestroyed)
            return;
        m_logicDestroyed = true;
        for (auto &script : scripts)
            script->OnDestroy();
        scripts.clear();
    }

    template <typename T>
    T *GetScrip() const
    {
        for (auto &script : scripts)
        {
            T *target = dynamic_cast<T *>(script.get());
            if (target != nullptr)
                return target;
        }
        return nullptr;
    }

    ~ScriptComponent() override
    {
        ExcuteOnDestroy();
    }
};