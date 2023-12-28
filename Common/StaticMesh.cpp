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
    // 파일 경로로부터 FBX 파일 이름 추출
    size_t lastSlash = path.find_last_of('/');
    size_t lastDot = path.find_last_of('.');

    if (lastSlash != std::string::npos && lastDot != std::string::npos)
    {
        std::wstring finalFilePath(path.begin(), path.end());
        m_fileName = finalFilePath.substr(lastSlash + 1, lastDot - lastSlash - 1);
    }

    // FBX 파일 경로를 scene에 바인딩
    Assimp::Importer importer;
    unsigned int importFlags = aiProcess_Triangulate |      // 삼각형으로 변환
        aiProcess_GenNormals |                              // 노말 생성
        aiProcess_GenUVCoords |                             // UV 생성
        aiProcess_CalcTangentSpace |                        // 탄젠트 생성
        aiProcess_LimitBoneWeights |                        // 본의 영향을 받는 정점의 최대 개수를 4개로 제한
        aiProcess_ConvertToLeftHanded;                      // 왼손 좌표계로 변환
    importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, 0);

    const aiScene* scene = importer.ReadFile(path, importFlags);

    // Mesh, Material 정보 Create
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
