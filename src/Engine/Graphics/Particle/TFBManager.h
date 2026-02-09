#pragma once
#include "Engine/Graphics/ShaderWrapper.h"
#include "GPUParticleBuffer.h"
class GameWorld;
class TFBManager
{
public:
    TFBManager() = default;
    void Simulate(GameWorld &gameWorld, Texture2D &dataTex, int maxParticles, ShaderWrapper &shader, GPUParticleBuffer &buffer, int count, float dt);
};