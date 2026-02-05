#pragma once
#include "IParticleInitializer.h"
#include "Engine/Graphics/ShaderWrapper.h"
#include <functional>
#include <unordered_map>
#include <string>
#include <memory>
#include <iostream>

class ParticleFactory
{
public:
    using InitializerCreator = std::function<std::unique_ptr<IParticleInitializer>()>;

    void Register(const std::string &name, InitializerCreator creator)
    {
        m_initializerRegistry[name] = creator;
    }
    std::unique_ptr<IParticleInitializer> CreatorInitializer(const std::string &name) const
    {
        auto it = m_initializerRegistry.find(name);
        if (it != m_initializerRegistry.end())
        {
            return it->second();
        }
        std::cout << "[ParticleModuleFactory] Error: No initializer with name: " << name << std::endl;
        return nullptr;
    }

private:
    std::unordered_map<std::string, InitializerCreator> m_initializerRegistry;
};