#pragma once
#include "SceneComponent.h"
#include "StaticMeshInstance.h"

class StaticMeshResource;
class MaterialTexture;

class EnvironmentMeshComponent :
    public SceneComponent
{
public:
    EnvironmentMeshComponent();
    ~EnvironmentMeshComponent();

public:
    std::shared_ptr<StaticMeshResource> m_resource;
    std::shared_ptr<MaterialTexture> m_envTexture;
    std::shared_ptr<MaterialTexture> m_envIBLDiffuseTexture;
    std::shared_ptr<MaterialTexture> m_envIBLSpecularTexture;
    std::shared_ptr<MaterialTexture> m_envIBLBRDFTexture;

    StaticMeshInstance m_meshInstance;

    std::string m_fbxPath;
    std::wstring m_envTexturePath;
    std::wstring m_envIBLDiffusePath;
    std::wstring m_envIBLSpecularPath;
    std::wstring m_envIBLBRDFPath;

public:
    bool ReadEnvironmentMesh(std::string filePath);
    bool ReadEnvironmentTexture(std::wstring filePath);
    bool ReadEnvironmentDiffuseTexture(std::wstring filePath);
    bool ReadEnvironmentSpecularTexture(std::wstring filePath);
    bool ReadEnvironmentBRDFTexture(std::wstring filePath);
};

