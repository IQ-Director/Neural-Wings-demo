#include "Engine/Graphics/Particle/IParticleInitializer.h"
#include "Engine/Math/Math.h"
#include "Engine/Utils/JsonParser.h"
#include <nlohmann/json.hpp>
using json = nlohmann::json;
class SPHInit : public IParticleInitializer
{
public:
    Vector3f spawnPos[3];
    Vector3f v[3];
    Vector2f size;

    float maxLife = 1.0f;
    void LoadConfig(const json &config) override
    {
        spawnPos[0] = JsonParser::ToVector3f(config["offset1"]);
        spawnPos[1] = JsonParser::ToVector3f(config["offset2"]);
        spawnPos[2] = JsonParser::ToVector3f(config["offset3"]);
        v[0] = JsonParser::ToVector3f(config["velocity1"]);
        v[1] = JsonParser::ToVector3f(config["velocity2"]);
        v[2] = JsonParser::ToVector3f(config["velocity3"]);
        size = JsonParser::ToVector2f(config["size"]);
        maxLife = config["maxLife"];
    }
    virtual int BurstCount() override
    {
        std::cout << "Collision Burst Count: " << 3 << std::endl;
        return 3;
    };
    void Initialize(std::vector<GPUParticle> &gpuParticles, size_t start, size_t count) override
    {
        for (size_t i = start; i < start + count; ++i)
        {
            if (i >= gpuParticles.size())
                break;
            auto &p = gpuParticles[i];
            p.position = spawnPos[i]; // 在碰撞点的坐标系
            p.velocity = v[i];
            p.acceleration = Vector3f::ZERO;
            p.color = Vector4f(1.0f, 0.0f, 0.0f, 1.0f);
            p.size = size;
            p.life = maxLife;
            p.rotation = 0.0f;
            p.randomID = std::rand() % 1000;
            p.ID = i;
        }
    }
};
