#include "Engine/Graphics/Particle/IParticleInitializer.h"
#include "Engine/Math/Math.h"
#include "Engine/Utils/JsonParser.h"
#include <nlohmann/json.hpp>
using json = nlohmann::json;
class SPHInit : public IParticleInitializer
{
public:
    Vector2f size;
    Vector3f spacing;
    Vector3f counts;
    int side;
    float maxLife = 1.0f;
    void LoadConfig(const json &config) override
    {
        maxLife = config["maxLife"];
        spacing = JsonParser::ToVector3f(config["spacing"]);
        counts = JsonParser::ToVector3f(config["counts"]);
        size = JsonParser::ToVector2f(config["size"]);
    }
    int BurstCount() override
    {
        std::cout << "Collision Burst Count: " << (int)counts.x() * counts.y() * counts.z() << std::endl;
        return (int)counts.x() * counts.y() * counts.z();
    };
    void Initialize(std::vector<GPUParticle> &gpuParticles, size_t start, size_t count) override
    {
        for (size_t i = start; i < start + count; ++i)
        {
            if (i >= gpuParticles.size())
                break;
            auto &p = gpuParticles[i];
            // i =gx+gy*x+gz*x*y
            int gx = i % (int)counts.x();
            int gy = (i / (int)counts.x()) % (int)counts.y();
            int gz = i / (int)counts.x() / counts.y();

            p.position = Vector3f(gx, gy, gz) & spacing; // 在碰撞点的坐标系
            p.velocity = Vector3f::ZERO;
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
