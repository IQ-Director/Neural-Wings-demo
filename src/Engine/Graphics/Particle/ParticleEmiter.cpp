#include "ParticleEmitter.h"
#include <nlohmann/json.hpp>
#include <memory>
#include <vector>
using json = nlohmann::json;

ParticleEmitter::ParticleEmitter(const json &config, const ParticleFactory &factory, ResourceManager &rm)
{
    LoadFromConfig(config, factory, rm);
}
void ParticleEmitter::ResetInsertionIndex()
{
    m_insertionIndex = 0;
}
void ParticleEmitter::LoadFromConfig(const json &config, const ParticleFactory &factory, ResourceManager &rm)
{
    if (config.contains("maxParticles"))
    {
        size_t maxP = config["maxParticles"];
        if (m_maxParticles != maxP)
        {
            m_maxParticles = maxP;
            ResetInsertionIndex();
            m_spawnBuffer.clear();
        }
    }
    m_emissionRate = config.value("emissionRate", m_emissionRate);
    m_maxLife = config.value("maxLife", m_maxLife);
    if (config.contains("space"))
    {
        std::string space = config["space"];
        simSpace = (space == "WORLD") ? SimulationSpace::WORLD : SimulationSpace::LOCAL;
    }
    if (config.contains("updateShader"))
    {
        std::string vsPath = config["updateShader"];
        std::vector<std::string> varyings =
            {
                "outPosition",
                "outVelocity",
                "outAcceleration",
                "outColor",
                "outSizeRotation",
                "outLifeRand",
            };
        m_updateShader = rm.GetTFBShader(vsPath, varyings);
    }
    else
    {
        std::cerr << "[ParticleEmitter]: No update shader specified!" << std::endl;
        return;
    }
    if (config.contains("initializers"))
    {
        m_initializers.clear();
        for (const auto &initConfig : config["initializers"])
        {
            std::string name = initConfig["name"];
            auto initObj = factory.CreatorInitializer(name);
            if (initObj)
            {
                if (initConfig.contains("params"))
                    initObj->LoadConfig(initConfig["params"]);
                m_initializers.push_back(std::move(initObj));
            }
        }
    }
    if (config.contains("renderMaterial"))
        m_renderMaterial.LoadFromConfig(config["renderMaterial"], rm);
}
void ParticleEmitter::Update(float deltaTime, const TransformComponent &ownerTf, GPUParticleBuffer &particleBuffer)
{
    m_lastDeltaTime += deltaTime;
    m_accumulator += deltaTime;
    int spawnCounts = (int)(m_accumulator * m_emissionRate);
    m_accumulator -= spawnCounts / (m_emissionRate);
    if (spawnCounts > 0)
    {
        m_lastDeltaTime = 0.0f;
        Spawn(spawnCounts, ownerTf, particleBuffer);
    }
}

void ParticleEmitter::Burst(const TransformComponent &ownerTf, GPUParticleBuffer &particleBuffer)
{
    size_t count = 0;
    for (auto &init : m_initializers)
        count += init->BurstCount();
    if (count > 0)
        Spawn(count, ownerTf, particleBuffer);
}

void ParticleEmitter::Spawn(int spawnCounts, const TransformComponent &ownerTf, GPUParticleBuffer &particleBuffer)
{

    if (spawnCounts > 1024)
        spawnCounts = 1024; // 防止卡顿
    m_spawnBuffer.assign(spawnCounts, GPUParticle());

    // 初始化
    for (auto &init : m_initializers)
        init->Initialize(m_spawnBuffer, 0, spawnCounts);

    // 随体系世界系处理
    if (simSpace == SimulationSpace::WORLD)
    {
        for (auto &particle : m_spawnBuffer)
        {
            particle.position = (ownerTf.rotation * (particle.position & ownerTf.scale)) + ownerTf.position;
            particle.velocity = ownerTf.rotation * particle.velocity;
            particle.acceleration = ownerTf.rotation * particle.acceleration;
        }
    }

    // 写入GPU
    // 循环写入，新粒子覆盖旧粒子
    if (m_insertionIndex + spawnCounts <= m_maxParticles)
    {
        particleBuffer.UpdateSubData(m_spawnBuffer, m_insertionIndex);
        m_insertionIndex += spawnCounts;
    }
    else
    {
        // 拆分两段
        size_t firstPartCount = m_maxParticles - m_insertionIndex;
        size_t secondPartCount = spawnCounts - firstPartCount;
        std::vector<GPUParticle> firstPart(m_spawnBuffer.begin(), m_spawnBuffer.begin() + firstPartCount);
        std::vector<GPUParticle> secondPart(m_spawnBuffer.begin() + firstPartCount, m_spawnBuffer.end());
        particleBuffer.UpdateSubData(firstPart, m_insertionIndex);
        particleBuffer.UpdateSubData(secondPart, 0);

        m_insertionIndex = secondPartCount;
    }
    if (m_insertionIndex >= m_maxParticles)
        m_insertionIndex = 0;
}

void ParticleEmitter::AddInitializer(std::shared_ptr<IParticleInitializer> initializer)
{
    m_initializers.push_back(initializer);
}

size_t ParticleEmitter::GetMaxParticles() const
{
    return m_maxParticles;
}
float ParticleEmitter::GetEmissionRate() const
{
    return m_emissionRate;
}
void ParticleEmitter::SetEmissionRate(float emissionRate)
{
    m_emissionRate = emissionRate;
}
bool ParticleEmitter::IsFinished() const
{
    return (m_emissionRate <= 0.001f) && (m_lastDeltaTime > m_maxLife);
}
void ParticleEmitter::SetMaxLife(float maxLife)
{
    m_maxLife = maxLife;
}

std::shared_ptr<ShaderWrapper> ParticleEmitter::GetUpdateShader() const
{
    return m_updateShader;
}

RenderMaterial &ParticleEmitter::GetRenderMaterial()
{
    return m_renderMaterial;
}

#include "rlgl.h"

#if defined(PLATFORM_WEB)
#include <GLES3/gl3.h>
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#else
#include "external/glad.h"
#endif

void ParticleEmitter::Render(GPUParticleBuffer &gpuBuffer, const Texture2D &sceneDepth, const Matrix4f &modelMat,
                             const Vector3f &viewPos, float realTime, float gameTime,
                             const Matrix4f &VP, const mCamera &camera)
{
    if (!m_renderMaterial.shader || !m_renderMaterial.shader->IsValid())
        return;

    rlDrawRenderBatchActive();
    rlEnableVertexArray(0);
    rlSetTexture(0);
    m_renderMaterial.shader->Begin();

    Vector3f right = camera.Right();
    Vector3f up = camera.Up();

    int texUnit = 0;
    for (const auto &[name, texture] : m_renderMaterial.customTextures)
    {
        if (texture.id > 0)
        {
            m_renderMaterial.shader->SetTexture(name, texture, texUnit);
            texUnit++;
        }
    }
    if (sceneDepth.id > 0)
    {
        m_renderMaterial.shader->SetTexture("u_sceneDepth", sceneDepth, texUnit);
        texUnit++;
    }

    m_renderMaterial.shader->SetVec2("u_resolution", Vector2f(GetScreenWidth(), GetScreenHeight()));
    m_renderMaterial.shader->SetFloat("u_near", camera.getNearPlane());
    m_renderMaterial.shader->SetFloat("u_far", camera.getFarPlane());

    m_renderMaterial.shader->SetVec3("u_cameraRight", right);
    m_renderMaterial.shader->SetVec3("u_cameraUp", up);
    m_renderMaterial.shader->SetMat4("u_vp", VP);
    m_renderMaterial.shader->SetMat4("u_model", modelMat);
    m_renderMaterial.shader->SetAll(Matrix4f::identity(), Matrix4f::identity(), viewPos, realTime, gameTime,
                                    m_renderMaterial.baseColor,
                                    m_renderMaterial.customFloats,
                                    m_renderMaterial.customVector2,
                                    m_renderMaterial.customVector3,
                                    m_renderMaterial.customVector4);

    rlDisableBackfaceCulling();
    rlEnableDepthTest();
    rlDisableDepthMask();

    switch (m_renderMaterial.blendMode)
    {
    case BLEND_OPIQUE:
        rlDisableColorBlend();
        break;
    case BLEND_MULTIPLIED:
        rlSetBlendMode(BLEND_CUSTOM);
        rlSetBlendFactors(RL_DST_COLOR, RL_ZERO, RL_FUNC_ADD);
        break;
    case BLEND_SCREEN:
        rlSetBlendMode(BLEND_CUSTOM);
        rlSetBlendFactors(RL_ONE, RL_ONE_MINUS_SRC_COLOR, RL_FUNC_ADD);
        break;
    case BLEND_SUBTRACT:
        rlSetBlendMode(BLEND_CUSTOM);
        rlSetBlendFactors(RL_ONE, RL_ONE, RL_FUNC_REVERSE_SUBTRACT);
        break;
    default:
        BeginBlendMode(m_renderMaterial.blendMode);
        break;
    }

    gpuBuffer.BindForRender();

    glDisable(GL_RASTERIZER_DISCARD);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, (GLsizei)m_maxParticles);

    glBindVertexArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glActiveTexture(GL_TEXTURE0);
    rlEnableVertexArray(0);
    rlSetTexture(0);
    rlDisableTexture();
    rlEnableDepthMask();
    EndBlendMode();

    m_renderMaterial.shader->End();
    rlEnableColorBlend();

    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR)
    {
        std::cerr << "OpenGL error: " << err << std::endl;
    }
}

Matrix4f ParticleEmitter::GetRenderMatrix(const TransformComponent &parentTf) const
{
    if (simSpace == SimulationSpace::WORLD)
        return Matrix4f::identity();
    else
        return parentTf.GetTransformMatrix(); // local space
}