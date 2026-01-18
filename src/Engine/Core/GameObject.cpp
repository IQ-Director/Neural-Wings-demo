#include "GameObject.h"

unsigned int GameObject::s_nextID = 0;

GameObject::GameObject()
    : m_id(s_nextID++)
{
}
unsigned int GameObject::GetID() const
{
    return m_id;
}
void GameObject::Destroy()
{
    m_isWaitingDestroy = true;
}

bool GameObject::IsWaitingDestroy() const
{
    return m_isWaitingDestroy;
}