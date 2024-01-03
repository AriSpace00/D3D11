#include "pch.h"
#include "Helper.h"
#include "Material.h"
#include "D3DRenderManager.h"

#include <filesystem>

#include <Directxtk/WICTextureLoader.h>

#include <assimp/scene.h>
#include <assimp/types.h>

Material::Material()
{
}

Material::~Material()
{
}

void Material::Create(aiMaterial* material)
{
    // 경로 설정
    aiString texturePath;
    std::filesystem::path path;
    std::wstring folderPath;
    std::string name;
    std::wstring basePath = L"../Resource/FBXLoad_Test/texture";

    for (const auto& entry : std::filesystem::recursive_directory_iterator(basePath))
    {
        if (entry.is_regular_file() && (entry.path().extension() == L".png" || entry.path().extension() == L".jpg"))
        {
            // fbx 이름이랑 폴더 이름이랑 같으면 folderPath 생성
            folderPath = entry.path().parent_path().wstring();

            std::wstring extractedString;
            size_t lastSlash = folderPath.find_last_of('\\');
            size_t lastDot = folderPath.find_last_of('.');

            if (lastSlash != std::string::npos && lastDot != std::string::npos)
            {
                extractedString = folderPath.substr(lastSlash + 1, lastDot - lastSlash - 1);
            }

            if (m_fileName == extractedString)
            {
                name = material->GetName().C_Str();
                break;
            }
        }
    }

    // 텍스처 로드
    if (AI_SUCCESS == material->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath))
    {
        path = ToWString(std::string(texturePath.C_Str()));
        std::wstring finalPath = folderPath + L"/" + path.filename().wstring();
        DirectX::CreateWICTextureFromFile(D3DRenderManager::m_instance->m_device, finalPath.c_str(), nullptr, &m_diffuseRV);
    }

    if (AI_SUCCESS == material->GetTexture(aiTextureType_NORMALS, 0, &texturePath))
    {
        path = ToWString(std::string(texturePath.C_Str()));
        std::wstring finalPath = folderPath + L"/" + path.filename().wstring();
        DirectX::CreateWICTextureFromFile(D3DRenderManager::m_instance->m_device, finalPath.c_str(), nullptr, &m_normalRV);
    }

    if (AI_SUCCESS == material->GetTexture(aiTextureType_SPECULAR, 0, &texturePath))
    {
        path = ToWString(std::string(texturePath.C_Str()));
        std::wstring finalPath = folderPath + L"/" + path.filename().wstring();
        DirectX::CreateWICTextureFromFile(D3DRenderManager::m_instance->m_device, finalPath.c_str(), nullptr, &m_specularRV);
    }

    if (AI_SUCCESS == material->GetTexture(aiTextureType_EMISSIVE, 0, &texturePath))
    {
        path = ToWString(std::string(texturePath.C_Str()));
        std::wstring finalPath = folderPath + L"/" + path.filename().wstring();
        DirectX::CreateWICTextureFromFile(D3DRenderManager::m_instance->m_device, finalPath.c_str(), nullptr, &m_emissiveRV);
    }

    if (AI_SUCCESS == material->GetTexture(aiTextureType_OPACITY, 0, &texturePath))
    {
        path = ToWString(std::string(texturePath.C_Str()));
        std::wstring finalPath = folderPath + L"/" + path.filename().wstring();
        DirectX::CreateWICTextureFromFile(D3DRenderManager::m_instance->m_device, finalPath.c_str(), nullptr, &m_opacityRV);
    }

    if (AI_SUCCESS == material->GetTexture(aiTextureType_METALNESS, 0, &texturePath))
    {
        path = ToWString(std::string(texturePath.C_Str()));
        std::wstring finalPath = folderPath + L"/" + path.filename().wstring();
        DirectX::CreateWICTextureFromFile(D3DRenderManager::m_instance->m_device, finalPath.c_str(), nullptr, &m_metalicRV);
    }

    if (AI_SUCCESS == material->GetTexture(aiTextureType_SHININESS, 0, &texturePath))
    {
        path = ToWString(std::string(texturePath.C_Str()));
        std::wstring finalPath = folderPath + L"/" + path.filename().wstring();
        DirectX::CreateWICTextureFromFile(D3DRenderManager::m_instance->m_device, finalPath.c_str(), nullptr, &m_roughnessRV);
    }
}

void Material::SetFileName(const std::wstring& fileName)
{
    m_fileName = fileName;
}
