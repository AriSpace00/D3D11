#include "pch.h"
#include "EnvironmentActor.h"
#include "EnvironmentMeshComponent.h"

EnvironmentActor::EnvironmentActor()
{
	auto component = CreateComponent<EnvironmentMeshComponent>("EnvironmentMeshComponent");
	m_environmentMeshComponent = component.get();
	SetRootComponent(m_environmentMeshComponent);
}

EnvironmentActor::~EnvironmentActor()
{
}

void EnvironmentActor::OnBeginPlay()
{
	__super::OnBeginPlay();
}

void EnvironmentActor::OnEndPlay()
{
	__super::OnEndPlay();
}

EnvironmentMeshComponent* EnvironmentActor::GetEnvironmentMeshComponent()
{
	return m_environmentMeshComponent;
}
