#include "pch.h"
#include "StaticMeshActor.h"

#include "StaticMeshComponent.h"

StaticMeshActor::StaticMeshActor()
	: m_staticMeshComponent(nullptr)
{
	m_staticMeshComponent = CreateComponent<StaticMeshComponent>("StaticMeshComponent").get();
	SetRootComponent(m_staticMeshComponent);
}

StaticMeshActor::~StaticMeshActor()
{
}

void StaticMeshActor::OnBeginPlay()
{
	__super::OnBeginPlay();

	/*if (!m_filePath.empty())
	{
		m_staticMeshComponent->ReadResource(m_filePath);
	}*/
}

void StaticMeshActor::OnEndPlay()
{
	__super::OnEndPlay();
}

StaticMeshComponent* StaticMeshActor::GetStaticMeshComponent()
{
	return m_staticMeshComponent;
}
