#pragma once
#include "Engine/Math/Math.h"
#pragma pack(push, 1)
struct GPUParticle
{

    Vector3f position = Vector3f(0.0f, 0.0f, 0.0f); // loc 0:vec3
    float padding0 = 0.0f;

    Vector3f velocity = Vector3f(0.0f, 0.0f, 0.0f); // loc 1
    float padding1 = 0.0f;                          // padding to align to 16 bytes

    Vector3f acceleration = Vector3f(0.0f, 0.0f, 0.0f); // loc 2
    float padding2 = 0.0f;

    Vector4f color = Vector4f(1.0f, 1.0f, 1.0f, 1.0f); // loc 3

    Vector2f size = Vector2f(1.0f, 1.0f); // loc 4
    float rotation = 0.0f;                // loc 5
    float padding3 = 0.0f;

    Vector2f life = Vector2f(1.0f, 1.0f); // loc 6 (totalLife,remainingLife )
    unsigned int randomID = 0;            // loc 7
    unsigned int ID = 0;                  // loc 8
};
#pragma pack(pop)