#pragma once
#include "raylib.h"
#include "raymath.h"
#include "Engine/Math/Math.h"
class mCamera
{
public:
    mCamera();
    mCamera(Vector3f pos, Vector3f tar, Vector3f u, float f,
            float nearPlane,
            float farPlane,
            const CameraMode &mode = CAMERA_CUSTOM);
    Vector3f Right() const;

    Vector3f Direction() const;
    Vector3f Position() const;
    Vector3f Target() const;
    Vector3f Up() const;
    float Fovy() const;

    void setRight(Vector3f right);
    void setDirection(Vector3f dir);
    void setPosition(Vector3f pos);
    void setTarget(Vector3f tar);
    void setUp(Vector3f u);
    void setFovy(float fovy);
    void setProjection(int projection);
    void setNearPlane(float nearPlane);
    void setFarPlane(float farPlane);

    float getNearPlane() const;
    float getFarPlane() const;

    void UpdateFromDirection(Vector3f pos, Vector3f dir, Vector3f u, const CameraMode &mode = CAMERA_CUSTOM);
    void UpdateFromTarget(Vector3f pos, Vector3f tar, Vector3f u, const CameraMode &mode = CAMERA_CUSTOM);
    void UpdatemCamera(const CameraMode &mode = CAMERA_CUSTOM);
    void Rotate(float LookHorizontal, float LookVertical);

    Camera3D &GetRawCamera();
    const Camera3D &GetConstRawCamera() const;

private:
    Camera3D m_rawCamera;

    float m_nearPlane;
    float m_farPlane;
    Vector3f m_right;
    Vector3f m_direction;
    Vector3f m_position;
    Vector3f m_target;
    Vector3f m_up;
    float m_fovy;
    int m_projection; // 0:透视 1:正交
};