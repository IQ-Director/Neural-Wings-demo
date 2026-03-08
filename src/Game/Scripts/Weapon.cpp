#include "Weapon.h"
#include "Engine/Core/GameObject/GameObject.h"
#include "Engine/Core/GameWorld.h"
#include "Engine/System/Ray/mRay.h"
#include "Game/Events/CombatEvents.h"
#include <limits>
void BulletScript::OnFixedUpdate(float fixedDeltaTime)
{
    timer += fixedDeltaTime;
    if (timer >= lifeTime)
        owner->GetOwnerWorld()->GetPool("bullet").Recycle(owner);
}
void BulletScript::OnWake()
{

    timer = 0.0f;
}
void BulletScript::Initialize(const json &data)
{
    lifeTime = data.value("lifeTime", 2.0f);
}

// tracking bullet
void TrackingBulletScript::Initialize(const json &data)
{
    m_thrust = data.value("thrust", 40.0f);
    m_steerSensitivity = data.value("steer", 25.0f);
    m_lifeTime = data.value("lifeTime", 5.0f);
    m_damping = data.value("damping", 0.5f);
}

void TrackingBulletScript::OnWake()
{
    m_timer = 0.0f;
    m_target = nullptr;
}

void TrackingBulletScript::OnFixedUpdate(float dt)
{
    m_timer += dt;
    auto &rb = owner->GetComponent<RigidbodyComponent>();
    auto &tf = owner->GetComponent<TransformComponent>();
    if (m_timer >= m_lifeTime)
    {
        owner->GetOwnerWorld()->GetPool("missile").Recycle(owner);
        return;
    }

    rb.AddForce(tf.GetForward() * m_thrust);

    if (m_target && m_target->IsActive() && !m_target->IsWaitingDestroy())
    {
        auto &targetTf = m_target->GetComponent<TransformComponent>();
        auto &targetRb = m_target->GetComponent<RigidbodyComponent>();

        Vector3f missilePos = tf.GetWorldPosition();
        Vector3f targetPos = targetTf.GetWorldPosition();
        Vector3f relativePos = targetPos - missilePos;
        float distance = relativePos.Length();

        float missileSpeed = rb.velocity.Length();
        float tti = (missileSpeed > 1.0f) ? (distance / missileSpeed) : 0.0f;

        tti = std::min(tti, 2.0f);
        Vector3f predictedPos = targetPos + (targetRb.velocity * tti);

        Vector3f toPredictedTarget = (predictedPos - missilePos).Normalized();
        Vector3f currentForward = tf.GetForward();

        Vector3f torqueAxis = currentForward ^ toPredictedTarget;

        Vector3f steerTorque = torqueAxis * m_steerSensitivity;
        Vector3f dampingTorque = -rb.angularVelocity * m_damping;
        rb.AddTorque(steerTorque + dampingTorque);
    }
}

// mine bullet
void MineScript::Initialize(const json &data)
{
    m_delay = data.value("delay", 1.5f);
    m_explosionDamage = data.value("damage", 200.0f);
    m_detectionRadius = data.value("radius", 30.0f);
    m_expForce = data.value("expForce", 100.0f);
}

void MineScript::OnWake()
{
    m_timer = 0.0f;
    m_isArmed = false;
}
void MineScript::OnFixedUpdate(float dt)
{
    m_timer += dt;

    auto &rb = owner->GetComponent<RigidbodyComponent>();
    rb.velocity *= 0.58f;
    rb.angularVelocity *= 0.99f;

    if (!m_isArmed && m_timer >= m_delay)
        m_isArmed = true;
    if (!m_isArmed)
        return;

    Vector3f minePos = owner->GetComponent<TransformComponent>().GetWorldPosition();
    for (auto *gameObject : owner->GetOwnerWorld()->GetActivateGameObjects())
    {
        if (gameObject->GetTag() == "mine")
            continue;
        Vector3f pos = gameObject->GetComponent<TransformComponent>().GetWorldPosition();
        if ((pos - minePos).Length() < m_detectionRadius)
        {
            Explode(gameObject);
        }
    }
}

void MineScript::Explode(GameObject *target)
{
    auto &world = *owner->GetOwnerWorld();
    Vector3f pos = owner->GetComponent<TransformComponent>().GetWorldPosition();
    world.GetEventManager().Emit(DamageEvent(target, m_explosionDamage, pos));

    world.GetParticleSystem().Spawn("Explosion", pos);
    // world.GetAudioManager().PlaySpatial("Explosion_Large", pos);

    Vector3f targetPos = target->GetComponent<TransformComponent>().GetWorldPosition();
    Vector3f force = (targetPos - pos).Normalized() * m_expForce;
    if (target->HasComponent<RigidbodyComponent>())
    {
        auto &rb = target->GetComponent<RigidbodyComponent>();
        rb.AddForce(force * rb.mass);
    }

    world.GetPool("mine").Recycle(owner);
}

void WeaponScript::Initialize(const json &data)
{
    if (data.contains("velocity_0"))
        m_bulletVelocity_0 = data["velocity_0"];
    if (data.contains("velocity_1"))
        m_bulletVelocity_1 = data["velocity_1"];
    m_fireRate_0 = data.value("fireRate_0", 0.15f);
    m_fireRate_1 = data.value("fireRate_1", 0.15f);
    m_fireRate_2 = data.value("fireRate_2", 0.15f);
}
#include <random>
void WeaponScript::OnUpdate(float deltaTime)
{
    auto &input = owner->GetOwnerWorld()->GetInputManager();
    if (input.IsActionPressed("SwitchWeaponNext"))
    {
        bulletType = (bulletType + 1) % bulletTypeCount;

        std::cout << "Bullet Type: " << bulletType << std::endl;
    }
    else if (input.IsActionPressed("SwitchWeaponPrev"))
    {
        bulletType = (bulletType - 1 + bulletTypeCount) % bulletTypeCount;
        std::cout << "Bullet Type: " << bulletType << std::endl;
    }

    m_fireTimer += deltaTime;
    switch (bulletType)
    {
    case 0:
        Bullet0(input);
        break;
    case 1:
        Bullet1(input);
        break;
    case 2:
        Bullet2(input);
        break;
    }
}

void WeaponScript::Bullet2(const InputManager &input)
{
    if (input.IsActionDown("Fire"))
    {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::uniform_real_distribution<float> dis(0.0f, 1.0f);
        while (m_fireTimer > m_fireRate_2)
        {
            m_fireTimer -= m_fireRate_2;

            auto &tf = owner->GetComponent<TransformComponent>();
            auto &rb = owner->GetComponent<RigidbodyComponent>();
            Vector3f spawnPos = tf.GetWorldPosition() - tf.GetForward() * 6.0f - tf.GetUp() * 2.0f;
            GameObject *mine = owner->GetOwnerWorld()->GetPool("mine").Spawn("mine_" + std::to_string(rand()), "mine", spawnPos, tf.GetWorldRotation());
            if (mine)
            {
                auto &mineRb = mine->GetComponent<RigidbodyComponent>();
                mineRb.velocity = rb.velocity * 0.5f;
            }
        }
    }
    else
        m_fireTimer = m_fireRate_2 - std::numeric_limits<float>::min();
}
void WeaponScript::Bullet1(const InputManager &input)
{
    if (input.IsActionDown("Fire"))
    {
        auto *camera = owner->GetOwnerWorld()->GetCameraManager().GetCamera("follow_2");
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::uniform_real_distribution<float> dis(0.0f, 1.0f);
        while (m_fireTimer > m_fireRate_1)
        {
            m_fireTimer -= m_fireRate_1;

            auto &tf = owner->GetComponent<TransformComponent>();
            Vector3f spawnPos = tf.GetWorldPosition() + tf.GetForward() * 3.0f - tf.GetUp() * 3.5f;
            mRay aimRay(camera->Position(), camera->Direction());
            mRaycastHit hit = aimRay.Raycast(1000.0f, *owner->GetOwnerWorld(), owner);

            std::string name = "missile_" + std::to_string(rand());
            GameObject *missile = owner->GetOwnerWorld()->GetPool("missile").Spawn(name, "missile", spawnPos, tf.GetWorldRotation());
            if (missile)
            {
                auto &rb = missile->GetComponent<RigidbodyComponent>();
                rb.velocity = owner->GetComponent<RigidbodyComponent>().velocity + tf.GetForward() * m_bulletVelocity_1;

                auto *trackScript = missile->GetScript<TrackingBulletScript>();
                if (trackScript)
                {
                    if (hit.hit)
                    {
                        trackScript->SetTarget(hit.entity);
                        std::cout << "[Weapon]: Missile Locked on " << hit.entity->GetName() << std::endl;
                    }
                }
                // TODO: audio
            }
        }
    }
    else
        m_fireTimer = m_fireRate_1 - std::numeric_limits<float>::min();
}
void WeaponScript::Bullet0(const InputManager &input)
{
    if (input.IsActionDown("Fire"))
    {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::uniform_real_distribution<float> dis(0.0f, 1.0f);

        while (m_fireTimer > m_fireRate_0)
        {
            m_fireTimer -= m_fireRate_0;
            auto &tf = owner->GetComponent<TransformComponent>();
            float size = owner->GetComponent<RigidbodyComponent>().localAABB.max.z();
            float width = owner->GetComponent<RigidbodyComponent>().localAABB.max.x();
            Vector3f spawnVel = owner->GetComponent<RigidbodyComponent>().velocity;
            Vector3f spawnPos = tf.GetWorldPosition() + tf.GetForward() * (size + 0.5f + dis(gen) * 0.5f) + tf.GetRight() * width;
            // 修改名字以区分不同类型的子弹和owner
            std::string name = "bullet_" + std::to_string(rand());
            GameObject *bullet = owner->GetOwnerWorld()->GetPool("bullet").Spawn(name, "bullet", spawnPos, tf.GetWorldRotation());
            auto &rb = bullet->GetComponent<RigidbodyComponent>();
            rb.velocity = tf.GetForward() * m_bulletVelocity_0 + spawnVel;

            spawnPos = tf.GetWorldPosition() + tf.GetForward() * (size + 0.5f + dis(gen) * 0.5f) - tf.GetRight() * width;
            name = "bullet_" + std::to_string(rand());
            GameObject *bullet2 = owner->GetOwnerWorld()->GetPool("bullet").Spawn(name, "bullet", spawnPos, tf.GetWorldRotation());
            auto &rb2 = bullet2->GetComponent<RigidbodyComponent>();
            rb2.velocity = tf.GetForward() * m_bulletVelocity_0 + spawnVel;

            auto &audio = owner->GetComponent<AudioComponent>();

            float randomPitch = 0.3f + dis(gen) * 1.5f;
            float randomVol = 0.8f + dis(gen) * 0.2f;
            audio.Play("Fire", randomVol, randomPitch);

            // world->GetParticleSystem().Spawn(...);
        }
    }
    else
        m_fireTimer = m_fireRate_0 - std::numeric_limits<float>::min();
}