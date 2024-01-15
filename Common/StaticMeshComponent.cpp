#include "pch.h"
#include "D3DRenderManager.h"
#include "ResourceManager.h"
#include "StaticMesh.h"
#include "StaticMeshInstance.h"
#include "StaticMeshComponent.h"

StaticMeshComponent::StaticMeshComponent()
{
	D3DRenderManager::m_instance->m_staticMeshComponents.push_back(this);
	m_iterator = --D3DRenderManager::m_instance->m_staticMeshComponents.end();
}

StaticMeshComponent::~StaticMeshComponent()
{
	D3DRenderManager::m_instance->m_staticMeshComponents.erase(m_iterator);
}

void StaticMeshComponent::Update(float deltaTime)
{
	__super::Update(deltaTime);
}

void StaticMeshComponent::OnBeginPlay()
{
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
		m_meshInstances[i].Create(&m_resource->m_meshes[i], &m_worldTM, m_resource->GetMeshMaterial(i));
	}
}

