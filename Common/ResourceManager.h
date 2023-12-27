#pragma once

class StaticMeshResource;
class SkeletalMeshResource;
class Material;
class Animation;

class ResourceManager
{
public:
    ResourceManager();
    ~ResourceManager();

    static ResourceManager* m_instance;

public:
    std::map<std::string, std::weak_ptr<StaticMeshResource>> m_staticMeshMap;
    std::map<std::string, std::weak_ptr<SkeletalMeshResource>> m_skeletalMeshMap;
    std::map<std::string, std::weak_ptr<Material>> m_materialMap;
    std::map<std::string, std::weak_ptr<Animation>> m_animationMap;

    std::shared_ptr<StaticMeshResource> CreateStaticMeshResource(std::string filePath);
    std::shared_ptr<SkeletalMeshResource> CreateSkeletalMeshResource(std::string filePath);
    std::shared_ptr<Material> CreateMaterial(std::string filePath);
    std::shared_ptr<Animation> CreateAnimation(std::string filePath);
};                                            

