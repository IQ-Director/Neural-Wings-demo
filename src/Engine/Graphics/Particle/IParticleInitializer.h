#pragma once
#include "GPUParticle.h"
#include "Engine/Math/Math.h"
#include <vector>
#include <variant>
#include <string>
#include <unordered_map>

using ParticleParamValue = std::variant<float, int, Vector2f, Vector3f, Vector4f>;
class ParticleParams
{
public:
    void Set(const std::string &name, const ParticleParamValue &value)
    {
        m_params[name] = value;
    }
    template <typename T>
    T Get(const std::string &name, T defaultValue) const
    {
        auto it = m_params.find(name);
        if (it != m_params.end() && std::holds_alternative<T>(it->second))
            return std::get<T>(it->second);
        return defaultValue;
    }

private:
    std::unordered_map<std::string, ParticleParamValue> m_params;
};

class IParticleInitializer
{
public:
    virtual ~IParticleInitializer() = default;
    virtual int BurstCount() { return 0; };
    // 局部系初始化
    virtual void Initialize(std::vector<GPUParticle> &particles, size_t start, size_t count) = 0;
    virtual void LoadConfig(const nlohmann::json &config) = 0;
};
