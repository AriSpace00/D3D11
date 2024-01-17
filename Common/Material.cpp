#include "pch.h"
#include "Helper.h"
#include "Material.h"
#include "D3DRenderManager.h"
#include "ResourceManager.h"

#include <filesystem>

#include <DirectXTex.h>
#include <Directxtk/WICTextureLoader.h>

#include <assimp/scene.h>
#include <assimp/types.h>

MaterialTexture::MaterialTexture()
{
}

MaterialTexture::~MaterialTexture()
{
}

void MaterialTexture::Create(const std::wstring& filePath)
{
	DirectX::CreateWICTextureFromFile(
		D3DRenderManager::m_instance->m_device,
		filePath.c_str(), nullptr,
		&m_textureRV);
	m_filePath = filePath;
}

void MaterialTexture::Create(const DirectX::XMFLOAT4& value)
{
	VertexMaterial vertex;
	vertex.Diffuse.x = value.x;
	vertex.Diffuse.y = value.y;
	vertex.Diffuse.z = value.z;
	vertex.Diffuse.w = 1.0f;

	uint8_t* test = reinterpret_cast<uint8_t*>(&vertex.Diffuse);

	/// TODO textureRV�� nullptr��
	HRESULT hr = DirectX::CreateWICTextureFromMemory(
		D3DRenderManager::m_instance->m_device,
		D3DRenderManager::m_instance->m_deviceContext,
		test,
		sizeof(vertex.Diffuse),
		nullptr,
		&m_textureRV);

	assert(!FAILED(hr));
}

Material::Material()
{
}

Material::~Material()
{
}

void Material::Create(aiMaterial* material)
{
	/// 1. �ؽ�ó ���� ��ΰ� ���� ��
	// ��� ����
	aiString texturePath;
	std::filesystem::path path;
	std::wstring folderPath;
	std::string name;
	std::wstring basePath = L"../Resource/FBXLoad_Test/texture";

	for (const auto& entry : std::filesystem::recursive_directory_iterator(basePath))
	{
		if (entry.is_regular_file() && (entry.path().extension() == L".png" || entry.path().extension() == L".jpg"))
		{
			// fbx �̸��̶� ���� �̸��̶� ������ folderPath ����
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

	// �ؽ�ó �ε�
	if (AI_SUCCESS == material->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath))
	{
		path = ToWString(std::string(texturePath.C_Str()));
		std::wstring finalPath = folderPath + L"/" + path.filename().wstring();
		m_diffuseRV = ResourceManager::m_instance->CreateMaterial(finalPath);
	}
	else
	{
		/// 2. �ؽ�ó ���� ��ΰ� ���� ��
		/// TODO Diffuse ���� fbx �ȿ� ����Ǿ��ִ� �ٸ� �ؽ�ó�� �޾ƿ���
		aiColor3D color;
		if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_DIFFUSE, color))
		{
			VertexMaterial vertex;
			vertex.Diffuse.x = color.r;
			vertex.Diffuse.y = color.g;
			vertex.Diffuse.z = color.b;
			vertex.Diffuse.w = 1.0f;

			/*DirectX::XMFLOAT4 vertexMaterial;
			vertexMaterial.x = color.r;
			vertexMaterial.y = color.g;
			vertexMaterial.z = color.b;
			vertexMaterial.w = 1.0f;*/

			m_diffuseRV = ResourceManager::m_instance->CreateMaterial(vertex.Diffuse);
		}
	}

	if (AI_SUCCESS == material->GetTexture(aiTextureType_NORMALS, 0, &texturePath))
	{
		path = ToWString(std::string(texturePath.C_Str()));
		std::wstring finalPath = folderPath + L"/" + path.filename().wstring();
		m_normalRV = ResourceManager::m_instance->CreateMaterial(finalPath);
	}

	if (AI_SUCCESS == material->GetTexture(aiTextureType_SPECULAR, 0, &texturePath))
	{
		path = ToWString(std::string(texturePath.C_Str()));
		std::wstring finalPath = folderPath + L"/" + path.filename().wstring();
		m_specularRV = ResourceManager::m_instance->CreateMaterial(finalPath);
	}

	if (AI_SUCCESS == material->GetTexture(aiTextureType_EMISSIVE, 0, &texturePath))
	{
		path = ToWString(std::string(texturePath.C_Str()));
		std::wstring finalPath = folderPath + L"/" + path.filename().wstring();
		m_emissiveRV = ResourceManager::m_instance->CreateMaterial(finalPath);
	}

	if (AI_SUCCESS == material->GetTexture(aiTextureType_OPACITY, 0, &texturePath))
	{
		path = ToWString(std::string(texturePath.C_Str()));
		std::wstring finalPath = folderPath + L"/" + path.filename().wstring();
		m_opacityRV = ResourceManager::m_instance->CreateMaterial(finalPath);
	}

	if (AI_SUCCESS == material->GetTexture(aiTextureType_METALNESS, 0, &texturePath))
	{
		path = ToWString(std::string(texturePath.C_Str()));
		std::wstring finalPath = folderPath + L"/" + path.filename().wstring();
		m_metalicRV = ResourceManager::m_instance->CreateMaterial(finalPath);
	}

	if (AI_SUCCESS == material->GetTexture(aiTextureType_SHININESS, 0, &texturePath))
	{
		path = ToWString(std::string(texturePath.C_Str()));
		std::wstring finalPath = folderPath + L"/" + path.filename().wstring();
		m_roughnessRV = ResourceManager::m_instance->CreateMaterial(finalPath);
	}
}

void Material::SetFileName(const std::wstring& fileName)
{
	m_fileName = fileName;
}
