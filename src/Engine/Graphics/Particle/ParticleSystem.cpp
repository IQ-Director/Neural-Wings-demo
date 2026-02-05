#include "ParticleSystem.h"
#include "Engine/Core/GameWorld.h"
#include "Engine/Core/Components/ParticleEmitterComponent.h"
#include "Engine/Core/Components/TransformComponent.h"
#include "rlgl.h"

ParticleSystem::ParticleSystem()
{
    m_TFBManager = std::make_unique<TFBManager>();
}
ParticleSystem::~ParticleSystem()
{
    m_emitterBuffers.clear();
};

GPUParticleBuffer *ParticleSystem::GetOrCreateBuffer(std::shared_ptr<ParticleEmitter> emitter)
{
    auto it = m_emitterBuffers.find(emitter);
    if (it == m_emitterBuffers.end())
    {
        auto buff = std::make_unique<GPUParticleBuffer>(emitter->GetMaxParticles());
        GPUParticleBuffer *buffPtr = buff.get();
        m_emitterBuffers[emitter] = std::move(buff);
        return buffPtr;
    }

    if (it->second->GetMaxParticles() != emitter->GetMaxParticles())
    {
        std::cout << "[ParticleSystem]: Resize GPU Buffer: " << it->second->GetMaxParticles() << " -> " << emitter->GetMaxParticles() << std::endl;
        it->second = std::make_unique<GPUParticleBuffer>(emitter->GetMaxParticles());
        emitter->ResetInsertionIndex();
    }
    return it->second.get();
}

void ParticleSystem::Update(GameWorld &gameWorld, float dt)
{
    auto &entities = gameWorld.GetEntitiesWith<ParticleEmitterComponent, TransformComponent>();
    // 实体携带粒子
    for (auto &entity : entities)
    {
        auto &rec = entity->GetComponent<ParticleEmitterComponent>();
        auto &ownerTf = entity->GetComponent<TransformComponent>();
        if (!rec.activate)
            continue;
        for (auto &emitter : rec.emitters)
        {
            // 生成GPU粒子
            GPUParticleBuffer *buffer = GetOrCreateBuffer(emitter);
            emitter->Update(dt, ownerTf, *buffer);
            if (emitter->GetUpdateShader())
            {
                emitter->PrepareForces(ownerTf);
                m_TFBManager->Simulate(*(emitter->GetUpdateShader()), *buffer, (int)emitter->GetMaxParticles(), dt);
            }
        }
    }

    // 遗留粒子
    for (auto it = m_orphans.begin(); it != m_orphans.end();)
    {
        GPUParticleBuffer *buffer = GetOrCreateBuffer(it->emitter);

        if (it->emitter->GetUpdateShader())
        {
            it->emitter->PrepareForces(it->lastTransform);
            m_TFBManager->Simulate(*(it->emitter->GetUpdateShader()), *buffer, (int)it->emitter->GetMaxParticles(), dt);
        }
        if (it->emitter->IsFinished())
        {
            m_emitterBuffers.erase(it->emitter);
            it = m_orphans.erase(it);
        }
        else
            ++it;
    }
}

void ParticleSystem::RegisterOrphan(std::shared_ptr<ParticleEmitter> emitter, const TransformComponent &lastTf)
{
    emitter->SetEmissionRate(0.0f);
    m_orphans.push_back({emitter, lastTf});
}