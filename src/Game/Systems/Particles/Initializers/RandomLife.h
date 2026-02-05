#include "Engine/Graphics/Particle/IParticleInitializer.h"
class RandomLife : public IParticleInitializer
{
public:
    float minSpeed = 5.0f;
    float maxSpeed = 10.0f;

    void LoadConfig(const nlohmann::json &config) override
    {
        if (config.contains("minSpeed"))
            minSpeed = config["minSpeed"];
        if (config.contains("maxSpeed"))
            maxSpeed = config["maxSpeed"];
    }
    void Initialize(std::vector<GPUParticle> &gpuParticles, size_t start, size_t count) override
    {
        for (size_t i = start; i < start + count; ++i)
        {
            if (i >= gpuParticles.size())
                break;
            auto &p = gpuParticles[i];
            Vector3f randomDir = Vector3f::RandomSphere();
            float speed = static_cast<float>(rand()) / RAND_MAX * (maxSpeed - minSpeed) + minSpeed;
            p.velocity = randomDir * speed;
            p.position = Vector3f(0.0f, 0.0f, 0.0f); // 随体系下坐标
            p.life = Vector2f(2.0f, 2.0f);           // 2s寿命
            p.randomID = rand() % 10000;
            p.color = Vector4f(1.0f, 0.0f, 0.0f, 1.0f);
            p.size = Vector2f(1.0f, 1.0f);
        }
    }
};
