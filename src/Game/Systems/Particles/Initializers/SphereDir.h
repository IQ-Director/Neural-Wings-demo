#include "Engine/Graphics/Particle/IParticleInitializer.h"
#include "Engine/Math/Math.h"
#include "Engine/Utils/JsonParser.h"
// 球均匀分布，随机速度，均匀大小
class SphereDir : public IParticleInitializer
{
public:
    Vector3f offset;
    float minSpeed;
    float maxSpeed;
    Vector2f size;
    void LoadConfig(const nlohmann::json &config) override
    {
        offset = JsonParser::ToVector3f(config["offset"]);
        minSpeed = config["minSpeed"];
        maxSpeed = config["maxSpeed"];
        size = JsonParser::ToVector2f(config["size"]);
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
            p.position = offset; // 随体系下坐标
            p.randomID = rand() % 10000;
            p.color = Vector4f(1.0f, 0.0f, 0.0f, 1.0f);
            p.size = size;
            p.ID = i;
        }
    }
};
