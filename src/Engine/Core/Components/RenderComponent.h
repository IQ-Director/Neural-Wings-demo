#pragma once
#include "IComponent.h"
#include "raylib.h"
#include "Engine/Math/Math.h"
#include "Engine/Graphics/RenderMaterial.h"
#include <unordered_map>

struct renderAABB
{
    Vector3f min = FLT_MAX;
    Vector3f max = -FLT_MAX;
    renderAABB() = default;
    renderAABB(const Vector3f &min, const Vector3f &max) : min(min), max(max) {}
};
struct RenderComponent : public IComponent
{
    Model model = {0};
    // RenderMaterial material;
    std::unordered_map<int, std::vector<RenderMaterial>> meshPasses;
    // 用于没有指派材质的mesh
    RenderMaterial defaultMaterial;

    bool isVisible = true;
    bool showWires = false;
    bool showAxes = false;
    bool showCenter = false;
    bool showAngVol = false;
    bool showVol = false;

    Vector4f totalBaseColor = Vector4f(255.0f);  // 基础颜色
    Vector3f scale = Vector3f(1.0f, 1.0f, 1.0f); // 渲染缩放
    bool castShadows = true;
    renderAABB localAABB;

    RenderComponent() = default;
    ~RenderComponent() = default;
};