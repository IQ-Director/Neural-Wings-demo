#include "Engine/Graphics/Particle/IParticleInitializer.h"
class RandomLife : public IParticleInitializer
{
public:
    float minLife;
    float maxLife;

    void LoadConfig(const nlohmann::json &config) override
    {
        minLife = config["minLife"];
        maxLife = config["maxLife"];
    }
    void Initialize(std::vector<GPUParticle> &gpuParticles, size_t start, size_t count) override
    {
        for (size_t i = start; i < start + count; ++i)
        {
            if (i >= gpuParticles.size())
                break;
            auto &p = gpuParticles[i];
            float life = minLife + (maxLife - minLife) * ((float)rand()) / (float)RAND_MAX;
            p.life = Vector2f(1.0f, 1.0f) * life;
        }
    }
};
