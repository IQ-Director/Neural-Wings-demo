#pragma once
#include "RenderView/RenderViewer.h"
#include "raylib.h"
#include "Camera/mCamera.h"
#include "RenderMaterial.h"
#include "Engine/Math/Math.h"
#include "PostProcess/PostProcesser.h"

#include <memory>
#include <vector>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
class GameWorld;
class CameraManager;
struct Mesh;
struct Model;

class Renderer
{
public:
    Renderer();
    ~Renderer();
    void Init(const std::string &configViewPath, GameWorld &gameWorld);
    void RenderScene(GameWorld &world, CameraManager &cameraManager);

private:
    void RawRenderScene(GameWorld &gameWorld, CameraManager &cameraManager);
    void RawRenderParticle(GameWorld &gameWorld, CameraManager &cameraManager);

    void RenderSinglePass(const Mesh &mesh, const Model &model, const int &meshIdx, const RenderMaterial &pass, const Matrix4f &MVP, const Matrix4f &M, const mCamera &camera, GameWorld &gameWorld);
    void DrawWorldObjects(GameWorld &gameWorld, Camera3D &rawCamera, mCamera &camera, float aspect);
    void DrawParticle(GameWorld &gameWorld, mCamera &camera, float aspect);

    bool LoadViewConfig(const std::string &configPath, GameWorld &gameWorld);
    std::unique_ptr<RenderViewer> m_renderViewer;
    std::unique_ptr<PostProcesser> m_postProcesser;

    // Texture2D m_dummyDepth;
    // void CopyDepthBuffer(RenderTexture2D sourceRT, Texture2D targetDepth);
    // Debug
    void DrawCoordinateAxes(Vector3f position, Quat4f rotation, float axisLength, float thickness);
    void DrawVector(Vector3f position, Vector3f direction, float axisLength, float thickness);
};