#include "pch.h"
#include "Model.h"
#include "Node.h"

#include <memory>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

Model::Model()
    : m_Position(DirectX::XMMatrixIdentity())
    , m_Rotation(DirectX::XMMatrixIdentity())
    , m_Scale(DirectX::XMMatrixIdentity())
    , m_IsEvaluate(true)
{
}

Model::~Model()
{
    m_CBTransform->Release();
    m_CBMaterial->Release();
    m_CBMatrixPalette->Release();
    m_AlphaBlendState->Release();
}

void Model::ReadFile(ID3D11Device* device, const std::string& path)
{
    if (IsFileLoad)
    {
        return;
    }

    // 파일 경로로부터 FBX 파일 이름 추출
    size_t lastSlash = path.find_last_of('/');
    size_t lastDot = path.find_last_of('.');

    if (lastSlash != std::string::npos && lastDot != std::string::npos)
    {
        std::wstring finalFilePath(path.begin(), path.end());
        m_FileName = finalFilePath.substr(lastSlash + 1, lastDot - lastSlash - 1);
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

    // Mesh 정보 Create
    m_Meshes.resize(scene->mNumMeshes);
    for (unsigned int i = 0; i < scene->mNumMeshes; i++)
    {
        m_Meshes[i].Create(device, scene->mMeshes[i]);
    }

    // Node 정보 Create
    if (scene->mRootNode != nullptr)
    {
        Node* m_RootNode = new Node();
        m_RootNode->Create(device, this, scene, scene->mRootNode);
    }

    // Material 정보 Create
    m_Materials.resize(scene->mNumMaterials);
    for (unsigned int i = 0; i < scene->mNumMaterials; ++i)
    {
        m_Materials[i].SetFileName(m_FileName);
        m_Materials[i].Create(device, scene->mMaterials[i]);
    }

    IsFileLoad = true;

    importer.FreeScene();
}

void Model::Update(const float& deltaTime)
{
    // 노드의 WorldTM을 Mesh의 WorldTM에 업데이트
    for (int i = 0; i < m_Nodes.size(); i++)
    {
        m_Nodes[i]->Update(deltaTime, this);
    }
}

void Model::Render(ID3D11DeviceContext* deviceContext)
{
    deviceContext->PSSetConstantBuffers(2, 1, &m_CBMaterial);

    // Mesh Render
    for (int i = 0; i < /*m_Nodes.size()*/1; i++)
    {
        m_Nodes[i]->Render(deviceContext, this);
    }
}

void Model::SetTransform(Matrix position, Matrix rotation, Matrix scale)
{
    m_Nodes[0]->m_NodeLocalTM = scale * rotation * position;
    m_Position = position;
    m_Rotation = rotation;
    m_Scale = scale;
}
