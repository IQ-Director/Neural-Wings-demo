#pragma once
#include "RenderView.h"
#include "Camera/mCamera.h"
#include <vector>
#include <nlohmann/json.hpp>
using json = nlohmann::json;
class GameWorld;
class CameraManager;

class Renderer
{
public:
    Renderer() = default;

    void AddRenderView(const RenderView &view);
    void ClearRenderViews();
    void RenderScene(GameWorld &world, CameraManager &cameraManager);

    bool LoadViewConfig(const std::string &configPath);

private:
    RenderView ParseViews(const json &data);
    std::vector<RenderView> m_renderViews;

    void DrawWorldObjects(GameWorld &gameWorld, Camera3D &rawCamera, mCamera &camera, float aspect);
};