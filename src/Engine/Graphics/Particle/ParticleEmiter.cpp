#include "ParticleEmitter.h"
#include <nlohmann/json.hpp>
#include <memory>
#include <vector>
using json = nlohmann::json;

ParticleEmitter::ParticleEmitter(const json &config, const ParticleFactory &factory, ResourceManager &rm)
{
    LoadFromConfig(config, factory, rm);

    m_gpuBuffer = std::make_unique<GPUParticleBuffer>(m_maxParticles);
}
void ParticleEmitter::ResetInsertionIndex()
{
    m_insertionIndex = 0;
}
void ParticleEmitter::LoadFromConfig(const json &config, const ParticleFactory &factory, ResourceManager &rm)
{
    if (config.contains("maxParticles"))
    {
        size_t maxP = config["maxParticles"];
        if (m_maxParticles != maxP)
        {
            m_maxParticles = maxP;
            ResetInsertionIndex();
            m_spawnBuffer.clear();
        }
    }
    m_emissionRate = config.value("emissionRate", m_emissionRate);
    m_maxLife = config.value("maxLife", m_maxLife);
    if (config.contains("space"))
    {
        std::string space = config["space"];
        simSpace = (space == "WORLD") ? SimulationSpace::WORLD : SimulationSpace::LOCAL;
    }
    if (config.contains("updateShader"))
    {
        std::string vsPath = config["updateShader"];
        std::vector<std::string> varyings =
            {
                "outPosition",
                "outVelocity",
                "outAcceleration",
                "outColor",
                "outSize",
                "outRotation",
                "outLife",
                "outRandomID"};
        m_updateShader = rm.GetTFBShader(vsPath, varyings);
    }
    else
    {
        std::cerr << "[ParticleEmitter]: No update shader specified!" << std::endl;
        return;
    }
    if (config.contains("initializers"))
    {
        m_initializers.clear();
        for (const auto &initConfig : config["initializers"])
        {
            std::string name = initConfig["name"];
            auto initObj = factory.CreatorInitializer(name);
            if (initObj)
            {
                if (initConfig.contains("params"))
                {
                    initObj->LoadConfig(initConfig["params"]);
                }
                m_initializers.push_back(std::move(initObj));
            }
        }
    }
}
void ParticleEmitter::Update(float deltaTime, const TransformComponent &ownerTf, GPUParticleBuffer &particleBuffer)
{
    m_lastDeltaTime += deltaTime;
    m_accumulator += deltaTime;
    int spawnCounts = (int)(m_accumulator * m_emissionRate);
    m_accumulator -= spawnCounts / (m_emissionRate);
    if (spawnCounts > 0)
    {
        m_lastDeltaTime = 0.0f;
        Spawn(spawnCounts, ownerTf, particleBuffer);
    }
}

void ParticleEmitter::Spawn(int spawnCounts, const TransformComponent &ownerTf, GPUParticleBuffer &particleBuffer)
{

    if (spawnCounts > 128)
        spawnCounts = 128; // 防止卡顿
    m_spawnBuffer.assign(spawnCounts, GPUParticle());

    // 初始化
    for (auto &init : m_initializers)
        init->Initialize(m_spawnBuffer, 0, spawnCounts);

    // 随体系世界系处理
    if (simSpace == SimulationSpace::WORLD)
    {
        for (auto &particle : m_spawnBuffer)
        {
            particle.position = (ownerTf.rotation * (particle.position & ownerTf.scale)) + ownerTf.position;
            particle.velocity = ownerTf.rotation * particle.velocity;
            particle.acceleration = ownerTf.rotation * particle.acceleration;
        }
    }

    // 写入GPU
    // 循环写入，新粒子覆盖旧粒子
    if (m_insertionIndex + spawnCounts <= m_maxParticles)
    {
        particleBuffer.UpdateSubData(m_spawnBuffer, m_insertionIndex);
        m_insertionIndex += spawnCounts;
    }
    else
    {
        // 拆分两段
        size_t firstPartCount = m_maxParticles - m_insertionIndex;
        size_t secondPartCount = spawnCounts - firstPartCount;
        std::vector<GPUParticle> firstPart(m_spawnBuffer.begin(), m_spawnBuffer.begin() + firstPartCount);
        std::vector<GPUParticle> secondPart(m_spawnBuffer.begin() + firstPartCount, m_spawnBuffer.end());
        particleBuffer.UpdateSubData(firstPart, m_insertionIndex);
        particleBuffer.UpdateSubData(secondPart, 0);

        m_insertionIndex = secondPartCount;
    }
    if (m_insertionIndex >= m_maxParticles)
        m_insertionIndex = 0;
}

void ParticleEmitter::AddInitializer(std::shared_ptr<IParticleInitializer> initializer)
{
    m_initializers.push_back(initializer);
}

size_t ParticleEmitter::GetMaxParticles() const
{
    return m_maxParticles;
}
float ParticleEmitter::GetEmissionRate() const
{
    return m_emissionRate;
}
void ParticleEmitter::SetEmissionRate(float emissionRate)
{
    m_emissionRate = emissionRate;
}
bool ParticleEmitter::IsFinished() const
{
    return (m_emissionRate <= 0.001f) && (m_lastDeltaTime > m_maxLife);
}
void ParticleEmitter::SetMaxLife(float maxLife)
{
    m_maxLife = maxLife;
}

std::shared_ptr<ShaderWrapper> ParticleEmitter::GetUpdateShader() const
{
    return m_updateShader;
}