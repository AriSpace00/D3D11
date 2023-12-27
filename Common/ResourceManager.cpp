#include "pch.h"
#include "ResourceManager.h"

#include "Helper.h"

ResourceManager* ResourceManager::m_instance = nullptr;

ResourceManager::ResourceManager()
{
    assert(m_instance == nullptr);
    m_instance = this;
}

ResourceManager::~ResourceManager()
{
}

std::shared_ptr<StaticMeshResource> ResourceManager::CreateStaticMeshResource(std::string filePath)
{
    auto it = m_staticMeshMap.find(filePath);
    if (it != m_staticMeshMap.end())
    {
        std::shared_ptr<StaticMeshResource> resourcePtr = it->second.lock();
        if (resourcePtr)
        {
            return resourcePtr;
        }
        else
        {
            m_staticMeshMap.erase(it);
        }
    }

    std::filesystem::path path = ToWString(filePath);
    if (!std::filesystem::exists(path))
    {
        LOG_MESSAGEA("Error file not Found : %s", filePath.c_str());
        return nullptr;
    }

    std::shared_ptr<StaticMeshResource> resourcePtr = std::make_shared<StaticMeshResource>();
    resourcePtr->Create(filePath.c_str());
    m_staticMeshMap[filePath] = resourcePtr;
    LOG_MESSAGEA("Complete file : %s", filePath.c_str());
    return resourcePtr;
}

std::shared_ptr<SkeletalMeshResource> ResourceManager::CreateSkeletalMeshResource(std::string filePath)
{
}

std::shared_ptr<Material> ResourceManager::CreateMaterial(std::string filePath)
{
    auto it = m_materialMap.find(filePath);
    if (it != m_materialMap.end())
    {
        std::shared_ptr<Material> resourcePtr = it->second.lock();
        if (resourcePtr)
        {
            return resourcePtr;
        }
        else
        {
            m_materialMap.erase(it);
        }
    }

    std::filesystem::path path = ToWString(filePath);
    if (!std::filesystem::exists(path))
    {
        LOG_MESSAGEA("Error file not Found : %s", filePath.c_str());
        return nullptr;
    }

    std::shared_ptr<Material> resourcePtr = std::make_shared<Material>();
    resourcePtr->Create(filePath.c_str());
    m_materialMap[filePath] = resourcePtr;
    LOG_MESSAGEA("Complete file : %s", filePath.c_str());
    return resourcePtr;
}

std::shared_ptr<Animation> ResourceManager::CreateAnimation(std::string filePath)
{
}
