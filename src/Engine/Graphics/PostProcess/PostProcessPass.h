#pragma once
#include <string>
#include <memory>
#include "Engine/Graphics/RenderMaterial.h"
#include <vector>

struct PostRenderMaterial
{
    std::shared_ptr<ShaderWrapper> shader;

    Vector4f baseColor = Vector4f(255.0, 255.0, 255.0, 255.0);

    std::unordered_map<std::string, Texture2D> customTextures;

    std::unordered_map<std::string, float> customFloats;
    std::unordered_map<std::string, Vector2f> customVector2;
    std::unordered_map<std::string, Vector3f> customVector3;
    std::unordered_map<std::string, Vector4f> customVector4;
};

struct PostProcessPass
{
    std::string name;
    PostRenderMaterial material;
    std::string outputTarget;
    // input: shader变量名+纹理名
    // {"u_sceneTexture","sceneTexture"}
    std::vector<std::pair<std::string, std::string>> inputs;
};
