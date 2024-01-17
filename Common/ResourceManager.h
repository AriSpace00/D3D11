#pragma once
#include <memory>
#include <map>

class StaticMeshResource;
class SkeletalMeshResource;
class MaterialTexture;
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
    std::map<std::wstring, std::weak_ptr<MaterialTexture>> m_materialMap;
    std::map<std::string, std::weak_ptr<Animation>> m_animationMap;

    std::shared_ptr<StaticMeshResource> CreateStaticMeshResource(std::string filePath);
    std::shared_ptr<SkeletalMeshResource> CreateSkeletalMeshResource(std::string filePath);
    std::shared_ptr<MaterialTexture> CreateMaterial(std::wstring filePath);
    std::shared_ptr<MaterialTexture> CreateMaterial(const DirectX::XMFLOAT4& value);
    std::shared_ptr<Animation> CreateAnimation(std::string filePath);
};                                            

