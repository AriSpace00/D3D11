#include "pch.h"
#include "Actor.h"
#include "Component.h"

Actor::Actor()
	: m_owner(nullptr)
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

World* Actor::GetOwner()
{
	return m_owner;
}

void Actor::SetOwner(World* world)
{
	m_owner = world;
}

void Actor::SetRootComponent(SceneComponent* rootComponent)
{
	m_rootComponent = rootComponent;
}

SceneComponent* Actor::GetRootComponent() const
{
	return m_rootComponent;
}

void Actor::OnBeginPlay()
{
	for (auto& component : m_ownedComponents)
	{
		component->OnBeginPlay();
	}
}

void Actor::OnEndPlay()
{
	for (auto& component : m_ownedComponents)
	{
		component->OnEndPlay();
	}
}

void Actor::SetWorldPosition(const Vector3& position)
{
}

void Actor::SetWorldTransform(Matrix transform)
{
}
