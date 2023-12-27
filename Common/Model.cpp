#include "pch.h"
#include "Model.h"
#include "Node.h"

#include <memory>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

Model::Model()
    : m_position(DirectX::XMMatrixIdentity())
    , m_rotation(DirectX::XMMatrixIdentity())
    , m_scale(DirectX::XMMatrixIdentity())
    , m_isEvaluate(true)
{
}

Model::~Model()
{
    m_transformCB->Release();
    m_materialCB->Release();
    m_matrixPaletteCB->Release();
    m_alphaBlendState->Release();
}

void Model::ReadFile(ID3D11Device* device, const std::string& path)
{
    if (isFileLoad)
    {
        return;
    }

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

    // Mesh ���� Create
    m_meshes.resize(scene->mNumMeshes);
    for (unsigned int i = 0; i < scene->mNumMeshes; i++)
    {
        m_meshes[i].Create(device, scene->mMeshes[i]);
    }

    // Node ���� Create
    if (scene->mRootNode != nullptr)
    {
        Node* m_RootNode = new Node();
        m_RootNode->Create(device, this, scene, scene->mRootNode);
    }

    // Material ���� Create
    m_materials.resize(scene->mNumMaterials);
    for (unsigned int i = 0; i < scene->mNumMaterials; ++i)
    {
        m_materials[i].SetFileName(m_fileName);
        m_materials[i].Create(device, scene->mMaterials[i]);
    }

    isFileLoad = true;

    importer.FreeScene();
}

void Model::Update(const float& deltaTime)
{
    // ����� WorldTM�� Mesh�� WorldTM�� ������Ʈ
    for (int i = 0; i < m_nodes.size(); i++)
    {
        m_nodes[i]->Update(deltaTime, this);
    }
}

void Model::Render(ID3D11DeviceContext* deviceContext)
{
    deviceContext->PSSetConstantBuffers(2, 1, &m_materialCB);

    // Mesh Render
    for (int i = 0; i < /*m_Nodes.size()*/1; i++)
    {
        m_nodes[i]->Render(deviceContext, this);
    }
}

void Model::SetTransform(Matrix position, Matrix rotation, Matrix scale)
{
    m_nodes[0]->m_nodeLocalTM = scale * rotation * position;
    m_position = position;
    m_rotation = rotation;
    m_scale = scale;
}
