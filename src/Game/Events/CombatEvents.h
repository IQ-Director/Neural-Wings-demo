#pragma once
#include "Engine/Core/Events/Events.h"
#include "Engine/Core/GameObject/GameObject.h"

struct DamageEvent : public IEvent
{
    EVENT_TYPE(DamageEvent)

    GameObject *victim;
    float amount;
    Vector3f hitPoint;

    DamageEvent(GameObject *v, float a, Vector3f pt)
        : victim(v), amount(a), hitPoint(pt) {}
};

struct DeathEvent : public IEvent
{
    EVENT_TYPE(DeathEvent)
    GameObject *victim;
    DeathEvent(GameObject *v) : victim(v) {}
};
