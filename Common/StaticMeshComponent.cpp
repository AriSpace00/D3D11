#include "pch.h"
#include "D3DRenderManager.h"
#include "ResourceManager.h"
#include "StaticMesh.h"
#include "StaticMeshComponent.h"
#include "StaticMeshInstance.h"

StaticMeshComponent::StaticMeshComponent()
{
	/// TODO : D3DRenderManager에 Static Mesh Component 추가
}

StaticMeshComponent::~StaticMeshComponent()
{
	/// TODO : D3DRenderManager의 Static Mesh Component 제거
}

void StaticMeshComponent::Update(float deltaTime)
{
	SceneComponent::Update(deltaTime);
}

void StaticMeshComponent::OnBeginPlay()
{
	if (!m_filePath.empty())
	{
		ReadResource(m_filePath);
	}
}

void StaticMeshComponent::OnEndPlay()
{
}

bool StaticMeshComponent::ReadResource(std::string filePath)
{
	std::shared_ptr<StaticMeshResource> resourcePtr = ResourceManager::m_instance->CreateStaticMeshResource(filePath);
	if (resourcePtr == nullptr)
	{
		return false;
	}
	SetResource(resourcePtr);
	return true;
}

void StaticMeshComponent::SetResource(std::shared_ptr<StaticMeshResource> resource)
{
	assert(resource);
	m_resource = resource;

	m_meshInstances.resize(m_resource->m_meshes.size());
	for (int i = 0; i < m_resource->m_meshes.size(); i++)
	{
		m_meshInstances[i].Create(&m_resource->m_meshes[i],
			&m_worldTM,
			m_resource->GetMeshMaterial(i));
	}
}

