#include "pch.h"
#include "StaticMesh.h"
#include "Material.h"

#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

StaticMesh::StaticMesh()
{
}

StaticMesh::~StaticMesh()
{
}

void StaticMesh::CreateVertexBuffer(ID3D11Device* device, const vector<Vertex>& vertices, UINT vertexCount)
{
}

void StaticMesh::CreateIndexBuffer(ID3D11Device* device, const vector<UINT>& indices, UINT indexCount)
{
}

void StaticMesh::Create(ID3D11Device* device, aiMesh* mesh)
{
}

StaticMeshResource::StaticMeshResource()
{
}

StaticMeshResource::~StaticMeshResource()
{
}

void StaticMeshResource::CreateResource(ID3D11Device* device, const std::string& path)
{
    // ���� ��ηκ��� FBX ���� �̸� ����
    size_t lastSlash = path.find_last_of('/');
    size_t lastDot = path.find_last_of('.');

    if (lastSlash != std::string::npos && lastDot != std::string::npos)
    {
        std::wstring finalFilePath(path.begin(), path.end());
        m_fileName = finalFilePath.substr(lastSlash + 1, lastDot - lastSlash - 1);
    }

    // FBX ���� ��θ� scene�� ���ε�
    Assimp::Importer importer;
    unsigned int importFlags = aiProcess_Triangulate |      // �ﰢ������ ��ȯ
        aiProcess_GenNormals |                              // �븻 ����
        aiProcess_GenUVCoords |                             // UV ����
        aiProcess_CalcTangentSpace |                        // ź��Ʈ ����
        aiProcess_LimitBoneWeights |                        // ���� ������ �޴� ������ �ִ� ������ 4���� ����
        aiProcess_ConvertToLeftHanded;                      // �޼� ��ǥ��� ��ȯ
    importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, 0);

    const aiScene* scene = importer.ReadFile(path, importFlags);

    // Mesh, Material ���� Create
    m_meshes.resize(scene->mNumMeshes);
    for (unsigned int i = 0; i < scene->mNumMeshes; i++)
    {
        m_meshes[i].Create(device, scene->mMeshes[i]);
    }

    m_materials.resize(scene->mNumMaterials);
    for (unsigned int i = 0; i < scene->mNumMaterials; ++i)
    {
        m_materials[i].SetFileName(m_fileName);
        m_materials[i].Create(device, scene->mMaterials[i]);
    }
}
