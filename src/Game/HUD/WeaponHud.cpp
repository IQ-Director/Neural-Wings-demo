#include "WeaponHud.h"
#include "Engine/Core/GameWorld.h"
#include "Engine/Core/GameObject/GameObject.h"
#include "Engine/Network/Sync/NetworkSyncComponent.h"
#include "Game/Scripts/Weapon.h"

WeaponHud::WeaponHud(GameWorld *world) : m_world(world) {}

void WeaponHud::OnEnter()
{
    m_lastWeaponType = -1;
    m_displayTimer = 0.0f;
    m_alpha = 0.0f;
}

GameObject *WeaponHud::GetLocalPlayer() const
{
    if (!m_world)
        return nullptr;
    auto entities = m_world->GetEntitiesWith<NetworkSyncComponent>();
    for (auto *obj : entities)
    {
        if (obj->GetComponent<NetworkSyncComponent>().isLocalPlayer)
            return obj;
    }
    return nullptr;
}

void WeaponHud::Update(float deltaTime)
{
    GameObject *player = GetLocalPlayer();
    if (!player)
        return;

    auto *weapon = player->GetScript<WeaponScript>();
    if (!weapon)
        return;
    if (weapon->bulletType != m_lastWeaponType)
    {
        if (m_lastWeaponType != -1)
            m_displayTimer = m_maxDisplayTime;
        m_lastWeaponType = weapon->bulletType;
    }

    if (m_displayTimer > 0.0f)
    {
        m_displayTimer -= deltaTime;
        if (m_displayTimer < 0.0f)
            m_displayTimer = 0.0f;

        float elapsed = m_maxDisplayTime - m_displayTimer;
        if (elapsed < m_fadeTime)
            m_alpha = elapsed / m_fadeTime;
        else if (m_displayTimer < m_fadeTime)
            m_alpha = m_displayTimer / m_fadeTime;
        else
            m_alpha = 1.0f;
    }
    else
        m_alpha = 0.0f;
}
void WeaponHud::Draw()
{
    if (m_alpha <= 0.001f)
        return;

    GameObject *player = GetLocalPlayer();
    if (!player)
        return;

    auto *weapon = player->GetScript<WeaponScript>();
    if (!weapon)
        return;

    std::string weaponName = "UNKNOWN";
    Color weaponColor = LIME;

    switch (weapon->bulletType)
    {
    case 0:
        weaponName = "M61 VULCAN CANNON";
        weaponColor = LIME;
        break;
    case 1:
        weaponName = "AIM-9 SIDEWINDER";
        weaponColor = ORANGE;
        break;
    case 2:
        weaponName = "PROXIMITY MINE";
        weaponColor = RED;
        break;
    }

    int fontSize = 40;
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    const char *text = weaponName.c_str();
    int textWidth = MeasureText(text, fontSize);

    int posX = screenWidth / 2 - textWidth / 2;
    int posY = screenHeight - 80;

    DrawRectangle(posX - 10, posY - 5, textWidth + 20, fontSize + 10, Fade(BLACK, 0.6f * m_alpha));
    DrawRectangleLines(posX - 10, posY - 5, textWidth + 20, fontSize + 10, Fade(weaponColor, 0.5f * m_alpha));

    DrawText(text, posX + 2, posY + 2, fontSize, Fade(BLACK, m_alpha));
    DrawText(text, posX, posY, fontSize, Fade(weaponColor, m_alpha));
}