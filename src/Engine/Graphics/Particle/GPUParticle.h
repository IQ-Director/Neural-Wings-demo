#pragma once
#include "Engine/Math/Math.h"
#pragma pack(push, 1)
struct GPUParticle
{

    Vector3f position; // loc 0:vec3
    float padding0 = 0.0f;

    Vector3f velocity;     // loc 1
    float padding1 = 0.0f; // padding to align to 16 bytes

    Vector3f acceleration; // loc 2
    float padding2 = 0.0f;

    Vector4f color; // loc 3

    Vector2f size;  // loc 4
    float rotation; // loc 5
    float padding3 = 0.0f;

    Vector2f life;         // loc 6 (totalLife,remainingLife )
    unsigned int randomID; // loc 7
    float padding4 = 0.0f;
};
#pragma pack(pop)