#pragma once
#include "ParticleEmitter.h"
#include "TFBManager.h"
#include "GPUParticleBuffer.h"
#include <unordered_map>

class GameWorld;
class mCamera;

class ParticleSystem
{
public:
    ParticleSystem();
    ~ParticleSystem();

    void Update(GameWorld &gameWorld, float dt);
    void Render(GameWorld &gameWorld, mCamera &camera);
    // 若gameobject销毁，保留world发射器，并记录其最后位置
    void RegisterOrphan(std::shared_ptr<ParticleEmitter> emitter, const TransformComponent &lastTransform);

private:
    std::unordered_map<std::shared_ptr<ParticleEmitter>, std::unique_ptr<GPUParticleBuffer>> m_emitterBuffers; /// gameobject消失后残留的粒子发射器
    struct OrphanEmitter
    {
        std::shared_ptr<ParticleEmitter> emitter;
        TransformComponent lastTransform;
    };
    std::vector<OrphanEmitter> m_orphans;
    std::unique_ptr<TFBManager> m_TFBManager;

    // 确保缓冲存在
    GPUParticleBuffer *GetOrCreateBuffer(std::shared_ptr<ParticleEmitter> emitter);
};