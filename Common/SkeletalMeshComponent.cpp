#include "pch.h"
#include "D3DRenderManager.h"
#include "ResourceManager.h"
#include "SkeletalMesh.h"
#include "SkeletalMeshInstance.h"
#include "SkeletalMeshComponent.h"

SkeletalMeshComponent::SkeletalMeshComponent()
{
	D3DRenderManager::m_instance->m_skeletalMeshComponents.push_back(this);
	m_iterator = --D3DRenderManager::m_instance->m_skeletalMeshComponents.end();
}

SkeletalMeshComponent::~SkeletalMeshComponent()
{
	D3DRenderManager::m_instance->m_skeletalMeshComponents.erase(m_iterator);
}

void SkeletalMeshComponent::Update(float deltaTime)
{
	__super::Update(deltaTime);
}

void SkeletalMeshComponent::OnBeginPlay()
{
}

void SkeletalMeshComponent::OnEndPlay()
{
}

bool SkeletalMeshComponent::ReadResource(std::string filePath)
{
	std::shared_ptr<SkeletalMeshResource> resourcePtr = ResourceManager::m_instance->CreateSkeletalMeshResource(filePath);
	if (resourcePtr == nullptr)
	{
		return false;
	}
	SetResource(resourcePtr);
	return true;
}

void SkeletalMeshComponent::SetResource(std::shared_ptr<SkeletalMeshResource> resource)
{
	assert(resource);
	m_resource = resource;

	m_meshInstances.resize(m_resource->m_meshes.size());
	for (int i = 0; i < m_resource->m_meshes.size(); i++)
	{
		m_meshInstances[i].Create(&m_resource->m_meshes[i], &m_worldTM, m_resource->GetMeshMaterial(i));
	}
}
