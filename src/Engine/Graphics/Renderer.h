#pragma once
#include "RenderView/RenderViewer.h"
#include "raylib.h"
#include "Camera/mCamera.h"
#include "RenderMaterial.h"
#include "Engine/Math/Math.h"
#include "PostProcess/PostProcesser.h"
#include "Skybox/Skybox.h"
#include "Lighting/LightingManager.h"

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
    void SetSkybox(const std::string &skyboxPath, Vector4f tintColor, GameWorld &gameWorld);
    void RenderScene(GameWorld &world, CameraManager &cameraManager);

    Skybox *GetSkybox() { return m_skybox.get(); }

    void Update(GameWorld &gameworld);

    static RenderTexture2D LoadRT(int width, int height, PixelFormat format);

private:
    std::unique_ptr<Skybox> m_skybox;
    bool m_useSkybox = false;

    void RawRenderScene(GameWorld &gameWorld, CameraManager &cameraManager);
    void RawRenderParticle(GameWorld &gameWorld, CameraManager &cameraManager);

    void RenderSinglePass(const Mesh &mesh, const Model &model, const int &meshIdx, const RenderMaterial &pass,
                          const Matrix4f &matProj, const Matrix4f &matView, const Matrix4f &MVP, const Matrix4f &M,
                          const mCamera &camera, GameWorld &gameWorld, const Vector4f &totalBaseColor);
    void DrawWorldObjects(GameWorld &gameWorld, Camera3D &rawCamera, mCamera &camera, float aspect);
    void DrawParticle(GameWorld &gameWorld, mCamera &camera, float aspect);

    bool LoadViewConfig(const std::string &configPath, GameWorld &gameWorld);
    std::unique_ptr<RenderViewer> m_renderViewer;
    std::unique_ptr<PostProcesser> m_postProcesser;
    std::unique_ptr<LightingManager> m_lightingManager;
    // Texture2D m_dummyDepth;
    // void CopyDepthBuffer(RenderTexture2D sourceRT, Texture2D targetDepth);
    // Debug
    void DrawCoordinateAxes(Vector3f position, Quat4f rotation, float axisLength, float thickness);
    void DrawVector(Vector3f position, Vector3f direction, float axisLength, float thickness);
    void DrawHitbox(GameWorld &gameWorld, CameraManager &cameraManager);
    void DrawAABB(GameWorld &gameWorld, CameraManager &cameraManager);
    void DrawRenderAABB(GameWorld &gameWorld, CameraManager &cameraManager);
};

struct Plane
{
    Vector3f normal;
    float distance;

    float DistanceToPoint(const Vector3f &p) const
    {
        return normal * p + distance;
    }
    void Normalize()
    {
        float mag = normal.Length();
        if (mag > 0.0f)
        {
            normal /= mag;
            distance /= mag;
        }
    }
};

class Frustum
{
public:
    void Extract(const Matrix4f &m)
    {
        // 左平面
        m_planes[0].normal.x() = m(3, 0) + m(0, 0);
        m_planes[0].normal.y() = m(3, 1) + m(0, 1);
        m_planes[0].normal.z() = m(3, 2) + m(0, 2);
        m_planes[0].distance = m(3, 3) + m(0, 3);

        // 右平面
        m_planes[1].normal.x() = m(3, 0) - m(0, 0);
        m_planes[1].normal.y() = m(3, 1) - m(0, 1);
        m_planes[1].normal.z() = m(3, 2) - m(0, 2);
        m_planes[1].distance = m(3, 3) - m(0, 3);

        // 下平面
        m_planes[2].normal.x() = m(3, 0) + m(1, 0);
        m_planes[2].normal.y() = m(3, 1) + m(1, 1);
        m_planes[2].normal.z() = m(3, 2) + m(1, 2);
        m_planes[2].distance = m(3, 3) + m(1, 3);

        // 上平面
        m_planes[3].normal.x() = m(3, 0) - m(1, 0);
        m_planes[3].normal.y() = m(3, 1) - m(1, 1);
        m_planes[3].normal.z() = m(3, 2) - m(1, 2);
        m_planes[3].distance = m(3, 3) - m(1, 3);

        // 近平面
        m_planes[4].normal.x() = m(3, 0) + m(2, 0);
        m_planes[4].normal.y() = m(3, 1) + m(2, 1);
        m_planes[4].normal.z() = m(3, 2) + m(2, 2);
        m_planes[4].distance = m(3, 3) + m(2, 3);

        // 远平面
        m_planes[5].normal.x() = m(3, 0) - m(2, 0);
        m_planes[5].normal.y() = m(3, 1) - m(2, 1);
        m_planes[5].normal.z() = m(3, 2) - m(2, 2);
        m_planes[5].distance = m(3, 3) - m(2, 3);

        for (int i = 0; i < 6; i++)
            m_planes[i].Normalize();
    }

    bool IsBoxVisible(const renderAABB &box) const
    {
        Vector3f min = box.min;
        Vector3f max = box.max;

        for (int i = 0; i < 6; i++)
        {
            Vector3f p = min;
            if (m_planes[i].normal.x() >= 0)
                p.x() = max.x();
            if (m_planes[i].normal.y() >= 0)
                p.y() = max.y();
            if (m_planes[i].normal.z() >= 0)
                p.z() = max.z();

            if (m_planes[i].DistanceToPoint(p) < 0)
            {
                return false;
            }
        }
        return true; // 可能可见
    }

private:
    Plane m_planes[6];
};