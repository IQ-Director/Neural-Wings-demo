#pragma once
#include "GPUParticle.h"
#include "Engine/Graphics/ShaderWrapper.h"
#include <vector>

class GPUParticleBuffer
{
public:
    GPUParticleBuffer(size_t maxParticles, const ShaderWrapper &shader);
    ~GPUParticleBuffer();
    GPUParticleBuffer(const GPUParticleBuffer &) = delete;
    // 交换读写缓冲
    void Swap();

    unsigned int GetReadVAO() const { return m_vaos[m_readIdx]; }
    unsigned int GetReadVBO() const { return m_vbos[m_readIdx]; }

    unsigned int GetWriteVAO() const { return m_vaos[m_readIdx ^ 1]; }
    unsigned int GetWriteVBO() const { return m_vbos[m_readIdx ^ 1]; }

    // CPU注入接口
    void UpdateSubData(const std::vector<GPUParticle> &newParticles, size_t offset);

    size_t GetMaxParticles() const { return m_maxParticles; }

    void BindForRender();

private:
    void SetupBuffers();

    size_t m_maxParticles;
    unsigned int m_vbos[2]; // Buffer A,B
    unsigned int m_vaos[2]; //  VAO A,B
    int m_readIdx = 0;      // 0 -> A, 1 -> B

    unsigned int m_renderVAOS[2] = {0};
    unsigned int m_quadVBO = 0;
    void SetupRenderVBO(const Shader &renderShader);
};