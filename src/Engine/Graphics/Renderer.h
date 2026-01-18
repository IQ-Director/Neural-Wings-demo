#pragma once
#include "RenderView.h"
#include <vector>

class GameWorld;
class CameraManager;

class Renderer {
public:
    Renderer() = default;

    void AddRenderView(const RenderView& view);
    void ClearRenderViews();
    void RenderScene(const GameWorld& world, CameraManager& cameraManager);

private:
    std::vector<RenderView> m_renderViews;

    void DrawWorldObjects(const GameWorld& world);
};