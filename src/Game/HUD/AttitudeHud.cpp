#include "AttitudeHud.h"
#include "Engine/Core/GameWorld.h"
#include "Engine/Core/Components/Components.h"
#include "Engine/Network/Client/NetworkClient.h"
#include "Engine/Network/Sync/NetworkSyncComponent.h"
#include "Engine/System/Ray/mRay.h"
#include <cmath>

AttitudeHud::AttitudeHud(GameWorld *world) : m_world(world) {}

GameObject *AttitudeHud::GetLocalPlayer() const
{
    if (!m_world)
        return nullptr;
    auto entities = m_world->GetEntitiesWith<NetworkSyncComponent, TransformComponent>();
    for (auto *obj : entities)
    {
        if (obj->GetComponent<NetworkSyncComponent>().isLocalPlayer)
        {
            return obj;
        }
    }
    return nullptr;
}

void AttitudeHud::Draw()
{
    GameObject *player = GetLocalPlayer();
    if (!player)
        return;

    const auto &tf = player->GetComponent<TransformComponent>();
    const auto &rb = player->GetComponent<RigidbodyComponent>();
    auto *mainCam = m_world->GetCameraManager().GetMainCamera();
    if (!mainCam)
        return;

    float airspeed = rb.velocity.Length();
    float speedFactor = std::clamp(airspeed / 100.0f, 0.0f, 1.0f);
    float dynamicPixelsPerDegree = m_pixelsPerDegree / (1.0f + speedFactor * 10.0f);
    float dynamicLadderWidth = m_ladderWidth * (1.0f - speedFactor * 0.9f);
    float dynamicLadderGap = m_ladderGap * (1.0f - speedFactor * 0.9f);

    Vector3f nosePos = tf.GetWorldPosition();
    Vector3f forward = tf.GetForward();
    Vector3f right = tf.GetRight();
    Vector3f up = tf.GetUp();

    float pitchRad = asinf(fmaxf(-1.0f, fminf(1.0f, forward.y())));
    float pitchDeg = pitchRad * RAD2DEG;

    float rollRad = atan2f(right.y(), up.y());

    float screenX = GetScreenWidth() / 2.0f;
    float screenY = GetScreenHeight() / 2.0f;

    mRay aimRay(nosePos, forward);
    mRaycastHit hit = aimRay.Raycast(100.0f, *m_world, player);
    Vector3f worldAimPoint;
    if (hit.hit)
    {
        worldAimPoint = hit.point;
    }
    else
    {
        worldAimPoint = nosePos + forward * 500.0f;
    }
    Vector3f toPoint = (worldAimPoint - mainCam->Position()).Normalized();
    float dotProduct = toPoint * mainCam->Direction();
    Vector2 aimScreenPos;
    if (dotProduct > 0)
    {
        aimScreenPos = GetWorldToScreen(worldAimPoint, mainCam->GetRawCamera());
        float reticleX = aimScreenPos.x;
        float reticleY = aimScreenPos.y;
        DrawRing({reticleX, reticleY},
                 5.0f * (speedFactor * 6.0f + 1.0f),
                 7.0f * (speedFactor * 5.0f + 1.0f),
                 0, 360, 0, Fade(RED, 0.8f));
        float lineLen = 15.0f * (speedFactor * 2.0f + 1.0f);
        DrawLineEx({reticleX - lineLen - 5, reticleY}, {reticleX - 5, reticleY}, 2.0f, RED);
        DrawLineEx({reticleX + 5, reticleY}, {reticleX + lineLen + 5, reticleY}, 2.0f, RED);
    }

    DrawRing({screenX, screenY}, 0.3f * (speedFactor * 30.0f + 1.0f), 1.0f * (speedFactor * 20.0f + 1.0f), 0, 360, 0, Fade(LIME, 0.8f));
    DrawLineEx({(screenX - 5 * (speedFactor * 20.0f + 1.0f)), screenY}, {(screenX - 2), screenY}, 2.0f, LIME);
    DrawLineEx({(screenX + 2), screenY}, {(screenX + 5 * (speedFactor * 20.0f + 1.0f)), screenY}, 2.0f, LIME);

    int centerStep = static_cast<int>(pitchDeg / dynamicLadderGap) * static_cast<int>(dynamicLadderGap);
    int dynamicFovLimit = static_cast<int>(m_fovLimit * (1.0f + speedFactor * 10.5f));

    float horizonWidth = dynamicLadderWidth * 6.0f;
    DrawHorizonLine(pitchDeg, rollRad, screenX, screenY, horizonWidth, GREEN, dynamicPixelsPerDegree);

    for (int step = centerStep - dynamicFovLimit; step <= centerStep + dynamicFovLimit; step += (int)m_ladderGap)
    {
        if (step > 90 || step < -90)
            continue;
        if (step == 0)
            continue;
        float width = dynamicLadderWidth;
        Color color = Fade(LIME, 0.7f);
        DrawPitchLine(pitchDeg, rollRad, (float)step, screenX, screenY, width, color, dynamicPixelsPerDegree);
    }
}
void AttitudeHud::DrawHorizonLine(float currentPitchDeg, float rollRad, float cx, float cy, float width, Color color, float pixelsPerDegree) const
{
    float linePitchDeg = 0.0f;
    float yOffset = -(linePitchDeg - currentPitchDeg) * pixelsPerDegree;
    float halfW = width / 2.0f;
    float centerGap = 80.0f;
    Vector2 p1_local_L = {-halfW, yOffset};
    Vector2 p2_local_L = {-centerGap, yOffset};
    Vector2 p1_local_R = {centerGap, yOffset};
    Vector2 p2_local_R = {halfW, yOffset};

    float c = cosf(rollRad);
    float s = sinf(rollRad);

    auto Rotate = [&](Vector2 p) -> Vector2
    {
        return {
            p.x * c - p.y * s + cx,
            p.x * s + p.y * c + cy};
    };

    DrawLineEx(Rotate(p1_local_L), Rotate(p2_local_L), 3.0f, color);
    DrawLineEx(Rotate(p1_local_R), Rotate(p2_local_R), 3.0f, color);
}
void AttitudeHud::DrawPitchLine(float currentPitchDeg, float rollRad, float linePitchDeg,
                                float cx, float cy, float width, Color color, float pixelsPerDegree) const
{
    float yOffset = -(linePitchDeg - currentPitchDeg) * pixelsPerDegree;

    const float centerGap = 60.0f;
    float lineLength = width / 2.0f;

    Vector2 p1_local = {centerGap, yOffset};
    Vector2 p2_local = {centerGap + lineLength, yOffset};

    float c = cosf(rollRad);
    float s = sinf(rollRad);

    auto Rotate = [&](Vector2 p) -> Vector2
    {
        return {
            p.x * c - p.y * s + cx,
            p.x * s + p.y * c + cy};
    };

    Vector2 p1_screen = Rotate(p1_local);
    Vector2 p2_screen = Rotate(p2_local);

    DrawLineEx(p1_screen, p2_screen, 2.0f, color);

    const char *text = TextFormat("%d", (int)linePitchDeg);
    int fontSize = 20;
    DrawTextPro(GetFontDefault(), text, p2_screen, {-10, 10}, -rollRad * RAD2DEG, (float)fontSize, 1.0f, WHITE);
}
