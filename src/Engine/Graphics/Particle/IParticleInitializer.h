#pragma once
#include "GPUParticle.h"
#include <vector>

class IParticleInitializer
{
public:
    virtual ~IParticleInitializer() = default;
    // 局部系初始化
    virtual void Initialize(std::vector<GPUParticle> &particles, size_t start, size_t count) = 0;
    virtual void LoadConfig(const nlohmann::json &config) {}
};