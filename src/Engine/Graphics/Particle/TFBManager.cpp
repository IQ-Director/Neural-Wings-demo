#include "TFBManager.h"
#include "rlgl.h"
#include "Engine/Core/GameWorld.h"
#if defined(PLATFORM_WEB)
#include <GLES3/gl3.h>
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#else
#include "external/glad.h"
#endif

void TFBManager::Simulate(GameWorld &gameWorld, Texture2D &dataTex, int maxParticles, ShaderWrapper &shader, GPUParticleBuffer &buffer, int count, float dt)
{
    if (count <= 0)
        return;

    float gameTime = gameWorld.GetTimeManager().GetGameTime();
    float realTime = gameWorld.GetTimeManager().GetRealTime();

    rlDrawRenderBatchActive();

    shader.Begin();

    // TODO:仿照RenderMaterial加入用户自定义uniform功能
    int texUnit = 0;

    shader.SetTexture("dataTex", dataTex, texUnit++);
    shader.SetInt("maxParticles", maxParticles);
    shader.SetFloat("deltaTime", dt);
    shader.SetFloat("gameTime", gameTime);

    shader.SetFloat("realTime", realTime);
    // for (auto const &[name, value] : customFloats)
    //     SetFloat(name, value);

    // for (auto const &[name, value] : customVector2)
    //     SetVec2(name, value);
    // for (auto const &[name, value] : customVector3)
    //     SetVec3(name, value);
    // for (auto const &[name, value] : customVector4)
    //     SetVec4(name, value);

    // 关闭光栅化，仅vs
    glEnable(GL_RASTERIZER_DISCARD);

    // 绑定读写缓冲
    glBindVertexArray(buffer.GetReadVAO());
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, buffer.GetWriteVBO());

    glBeginTransformFeedback(GL_POINTS);
    // draw call
    glDrawArrays(GL_POINTS, 0, count);
    glEndTransformFeedback();

    // 恢复状态
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, 0);
    glBindVertexArray(0);
    glDisable(GL_RASTERIZER_DISCARD);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    shader.End();

    // 交换缓冲区，本帧输出作为下帧输入
    buffer.Swap();
}