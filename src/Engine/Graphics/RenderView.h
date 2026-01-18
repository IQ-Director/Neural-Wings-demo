#pragma once
#include "raylib.h"
#include <string>

struct RenderView {
    std::string cameraName;       // 使用的摄像机
    Rectangle viewport;         // 渲染到屏幕的目标区域
    bool clearBackground = false; // 是否清理背景
    Color backgroundColor = BLACK;  // 如果清理，后使用的背景颜色

    // TODO：后处理shader
};