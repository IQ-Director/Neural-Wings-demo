#pragma once
#include <string>
#include "raylib.h"
#include "Engine/Math/Math.h"
#include <unordered_map>

class ShaderWrapper
{
public:
    ShaderWrapper(const std::string &vsPath, const std::string &fsPath);
    ~ShaderWrapper();

    ShaderWrapper(const ShaderWrapper &) = delete;
    ShaderWrapper &operator=(const ShaderWrapper &) = delete;

    void Begin() const;
    void End() const;

    // Uniform 接口
    void SetInt(const std::string &name, int value);
    void SetFloat(const std::string &name, float value);
    void SetVec3(const std::string &name, const Vector3f &value);
    void SetVec4(const std::string &name, const Vector4f &value);
    void SetMat4(const std::string &name, const Matrix4f &value);
    void SetTexture(const std::string &name, Texture2D texture, int unit);

    bool IsValid() const { return m_shader.id > 0; };
    Shader GetShader() const { return m_shader; }

private:
    int GetLocation(const std::string &name);
    Shader m_shader;
    std::unordered_map<std::string, int> m_locationCache;
};