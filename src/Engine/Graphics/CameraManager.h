#pragma once
#include "raylib.h"
#include <string>
#include <unordered_map>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class CameraManager {
public:
    CameraManager() = default;
    Camera3D& CreateCamera(const std::string& name);//默认相机
    bool CreateCameraFromConfig(const json& configData);

    bool RemoveCamera(const std::string& name);
    bool LoadConfig(const std::string& filePath);//加载多相机


    Camera3D* GetCamera(const std::string& name);
    Camera3D* GetMainCamera();
    void SetMainCamera(const std::string& name);

private:
    std::unordered_map<std::string, Camera3D> m_cameras;
    std::string m_mainCameraName;
};