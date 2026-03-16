#include "Engine/Graphics/Particle/IParticleInitializer.h"
#include "Engine/Math/Math.h"
#include "Engine/Utils/JsonParser.h"
#include <nlohmann/json.hpp>
#include <random>
using json = nlohmann::json;
class ExplosionInit : public IParticleInitializer
{
public:
    Vector2f size;
    int counts;
    float velocity;
    float maxLife = 1.0f;
    void LoadConfig(const json &config) override
    {
        maxLife = config["maxLife"];
        counts = config.value("counts", 100);
        velocity = config.value("velocity", 10.0f);
        size = JsonParser::ToVector2f(config["size"]);
    }
    int BurstCount() override
    {
        if (__SHOWINFO__)
            std::cout << "Explosion Burst Count: " << counts << std::endl;
        return counts;
    };
    void Initialize(std::vector<GPUParticle> &gpuParticles, size_t start, size_t count) override
    {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::uniform_real_distribution<float> dis(0.0f, 1.0f);

        for (size_t i = start; i < start + count; ++i)
        {
            if (i >= gpuParticles.size())
                break;
            auto &p = gpuParticles[i];

            p.position = Vector3f(0.0f, 0.0f, 0.0f); // 在碰撞点的坐标系
            p.velocity = Vector3f::RandomSphere() * velocity * dis(gen);
            p.acceleration = Vector3f::ZERO;
            p.color = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
            p.size = size;
            p.life = maxLife;
            p.rotation = 0.0f;
            p.randomID = std::rand() % 1000;
            p.ID = i;
        }
    }
};
