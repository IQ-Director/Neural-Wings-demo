#pragma once
#include "GPUParticleBuffer.h"
#include "IParticleInitializer.h"
#include "ParticleFactory.h"
#include "Engine/Graphics/ShaderWrapper.h"
#include "Engine/Core/Components/TransformComponent.h"
#include "Engine/System/Resource/ResourceManager.h"
#include <nlohmann/json.hpp>
#include <memory>
#include <vector>
using json = nlohmann::json;
enum class SimulationSpace
{
    LOCAL,
    WORLD
};

class ParticleEmitter
{
public:
    SimulationSpace simSpace = SimulationSpace::WORLD; // 默认世界系

    ParticleEmitter(const json &config, const ParticleFactory &factory, ResourceManager &rm);
    void LoadFromConfig(const json &config, const ParticleFactory &factory, ResourceManager &rm);
    void ResetInsertionIndex();
    void Update(float deltaTime, const TransformComponent &parentTf, GPUParticleBuffer &particleBuffer);
    void AddInitializer(std::shared_ptr<IParticleInitializer> initializer);
    void PrepareForces(const TransformComponent &parentTf);

    bool IsFinished() const;
    size_t GetMaxParticles() const;

    float GetEmissionRate() const;
    void SetEmissionRate(float rate);

    std::shared_ptr<ShaderWrapper> GetUpdateShader() const;
    void SetMaxLife(float life);

private:
    float m_maxLife = 5.0f;       // 粒子最大生命周期
    float m_lastDeltaTime = 0.0f; // 距离上一次发射粒子过去的时间

    void Spawn(int count, const TransformComponent &parentTf, GPUParticleBuffer &gpuBuffer);

    float m_emissionRate = 0.0f;
    float m_accumulator = 0.0f;
    size_t m_insertionIndex = 0; // 循环缓冲区写指针
    size_t m_maxParticles = 1000;

    std::shared_ptr<GPUParticleBuffer> m_gpuBuffer;
    std::shared_ptr<ShaderWrapper> m_updateShader;
    std::vector<std::shared_ptr<IParticleInitializer>> m_initializers;
    std::vector<GPUParticle> m_spawnBuffer; // 临时缓冲，传给GPU前组装数据
};