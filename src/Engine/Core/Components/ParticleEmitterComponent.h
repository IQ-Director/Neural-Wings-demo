#pragma once
#include "IComponent.h"
#include "Engine/Graphics/Particle/ParticleEmitter.h"
#include <memory>
struct ParticleEmitterComponent : public IComponent
{
    std::vector<std::shared_ptr<ParticleEmitter>> emitters;
    ParticleEmitterComponent() = default;
    void AddEmitter(std::shared_ptr<ParticleEmitter> emitter)
    {
        emitters.push_back(emitter);
    }
    // 是否开启
    bool activate = true;
};