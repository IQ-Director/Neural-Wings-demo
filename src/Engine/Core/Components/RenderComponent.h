#pragma once
#include "IComponent.h"
#include "raylib.h"
#include "Engine/Math/Math.h"
#include "Engine/Graphics/RenderMaterial.h"
struct RenderComponent : public IComponent
{
    Model model;
    RenderMaterial material;

    Vector3f scale = Vector3f(1.0f, 1.0f, 1.0f); // 渲染缩放
    RenderComponent() = default;
    ~RenderComponent() = default;
};