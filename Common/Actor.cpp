#include "pch.h"
#include "Actor.h"

Actor::Actor()
    : m_ownerWorld(nullptr)
{
}

Actor::~Actor()
{
}

void Actor::Update(float deltaTime)
{
}

void Actor::Render(ID3D11DeviceContext* deviceContext)
{
}

void Actor::SetRootComponent(SceneComponent* rootComponent)
{
    m_rootComponent = rootComponent;
}

SceneComponent* Actor::GetRootComponent() const
{
    return m_rootComponent;
}
