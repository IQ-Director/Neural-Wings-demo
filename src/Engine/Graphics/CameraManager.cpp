#include "CameraManager.h"
#include <fstream>
#include <iostream>

Camera3D& CameraManager::CreateCamera(const std::string& name) {
    return m_cameras[name];
}

bool CameraManager::CreateCameraFromConfig(const json& configData) {
    if (!configData.contains("name")) return false;

    std::string name = configData["name"];
    auto& cam = m_cameras[name];

    cam.position = Vector3{
        configData.value("position", json::array({0.f, 0.f, 0.f}))[0],
        configData.value("position", json::array({0.f, 0.f, 0.f}))[1],
        configData.value("position", json::array({0.f, 0.f, 0.f}))[2]
    };
    cam.target = Vector3{
        configData.value("target", json::array({0.f, 0.f, 0.f}))[0],
        configData.value("target", json::array({0.f, 0.f, 0.f}))[1],
        configData.value("target", json::array({0.f, 0.f, 0.f}))[2]
    };
    cam.up = Vector3{
        configData.value("up", json::array({0.f, 1.f, 0.f}))[0],
        configData.value("up", json::array({0.f, 1.f, 0.f}))[1],
        configData.value("up", json::array({0.f, 1.f, 0.f}))[2]
    };
    cam.fovy = configData.value("fovy", 60.0f);

    std::string proj = configData.value("projection", "PERSPECTIVE");
    if (proj == "ORTHOGRAPHIC") {
        cam.projection = CAMERA_ORTHOGRAPHIC;
    } else {
        cam.projection = CAMERA_PERSPECTIVE;
    }

    if (configData.value("isMain", false)) {
        SetMainCamera(name);
    }
    
    return true;
}

bool CameraManager::RemoveCamera(const std::string& name) {
    if (name == m_mainCameraName) {
        m_mainCameraName.clear();
    }
    return m_cameras.erase(name) > 0;
}

bool CameraManager::LoadConfig(const std::string& filePath) {
    m_cameras.clear();
    m_mainCameraName.clear();

    std::ifstream configFile(filePath);
    if (!configFile.is_open()) {
        std::cerr << "Error: [CameraManager] Could not open file: " << filePath << std::endl;
        return false;
    }
    
    json configJson;
    try {
        configFile >> configJson;
        if (configJson.contains("cameras") && configJson["cameras"].is_array()) {
            for (const auto& camConfig : configJson["cameras"]) {
                CreateCameraFromConfig(camConfig);
            }
        }
    } catch (json::parse_error& e) {
        std::cerr << "Error: [CameraManager] Failed to parse " << filePath << ": " << e.what() << std::endl;
        return false;
    }
    
    // 若没有主相机，设置第一个为默认主相机
    if (m_mainCameraName.empty() && !m_cameras.empty()) {
        SetMainCamera(m_cameras.begin()->first);
    }

    return true;
}

Camera3D* CameraManager::GetCamera(const std::string& name) {
    auto it = m_cameras.find(name);
    if (it != m_cameras.end()) {
        return &(it->second);
    }
    return nullptr;
}

Camera3D* CameraManager::GetMainCamera() {
    if (m_mainCameraName.empty()) return nullptr;
    return GetCamera(m_mainCameraName);
}

void CameraManager::SetMainCamera(const std::string& name) {
    if (m_cameras.count(name)) {
        m_mainCameraName = name;
    }
}