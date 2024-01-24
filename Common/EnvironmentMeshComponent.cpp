#include "pch.h"
#include "EnvironmentMeshComponent.h"
#include "ResourceManager.h"
#include "StaticMesh.h"

EnvironmentMeshComponent::EnvironmentMeshComponent()
{
}

EnvironmentMeshComponent::~EnvironmentMeshComponent()
{
}

bool EnvironmentMeshComponent::ReadEnvironmentMesh(std::string filePath)
{
	m_resource = ResourceManager::m_instance->CreateStaticMeshResource(filePath);
	assert(m_resource != nullptr);
	if (m_resource == nullptr)
		return false;
	m_meshInstance.Create(&m_resource->m_meshes[0], &m_worldTM, nullptr);
	m_fbxPath = filePath;
	return true;
}

bool EnvironmentMeshComponent::ReadEnvironmentTexture(std::wstring filePath)
{
	m_envTexture = ResourceManager::m_instance->CreateMaterial(filePath);
	assert(m_envTexture != nullptr);
	if (m_envTexture == nullptr)
		return false;
	m_envTexturePath = filePath;
	return true;
}

bool EnvironmentMeshComponent::ReadEnvironmentDiffuseTexture(std::wstring filePath)
{
	m_envIBLDiffuseTexture = ResourceManager::m_instance->CreateMaterial(filePath);
	assert(m_envIBLDiffuseTexture != nullptr);
	if (m_envIBLDiffuseTexture == nullptr)
		return false;
	m_envIBLDiffusePath = filePath;
	return true;
}

bool EnvironmentMeshComponent::ReadEnvironmentSpecularTexture(std::wstring filePath)
{
	m_envIBLSpecularTexture = ResourceManager::m_instance->CreateMaterial(filePath);
	assert(m_envIBLSpecularTexture != nullptr);
	if (m_envIBLSpecularTexture == nullptr)
		return false;
	m_envIBLSpecularPath = filePath;
	return true;
}

bool EnvironmentMeshComponent::ReadEnvironmentBRDFTexture(std::wstring filePath)
{
	m_envIBLBRDFTexture = ResourceManager::m_instance->CreateMaterial(filePath);
	assert(m_envIBLBRDFTexture != nullptr);
	if (m_envIBLBRDFTexture == nullptr)
		return false;
	m_envIBLBRDFPath = filePath;
	return true;
}
