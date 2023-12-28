#include "pch.h"
#include "StaticMeshComponent.h"

#include "ResourceManager.h"

StaticMeshComponent::StaticMeshComponent()
{
}

StaticMeshComponent::~StaticMeshComponent()
{
}

void StaticMeshComponent::Update(float deltaTime)
{
    SceneComponent::Update(deltaTime);
}

bool StaticMeshComponent::ReadResource(std::string filePath)
{
    std::shared_ptr<StaticMesh> resourcePtr = ResourceManager::m_instance->CreateStaticMeshResource(filePath);
    if (resourcePtr == nullptr)
    {
        return false;
    }
    SetResource(resourcePtr);
    return true;
}

void StaticMeshComponent::SetResource(std::shared_ptr<StaticMesh> resource)
{
}

