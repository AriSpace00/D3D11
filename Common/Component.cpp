#include "pch.h"
#include "Component.h"

Component::Component()
{
}

Component::~Component()
{
}

void Component::SetName(const std::string& name)
{
    m_name = name;
}

const std::string& Component::GetName() const
{
    return m_name;
}

void Component::SetOwner(Actor* owner)
{
    m_owner = owner;
}

Actor* Component::GetOwner()
{
    return m_owner;
}

void Component::Update(float deltaTime)
{
}
