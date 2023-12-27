#include "pch.h"
#include "Mesh.h"
#include "Bone.h"

#include <assimp/scene.h>
#include <map>

Mesh::Mesh()
{
}

Mesh::~Mesh()
{
}

void Mesh::CreateVertexBuffer(ID3D11Device* device, const vector<Vertex>& vertices, UINT vertexCount)
{
    D3D11_BUFFER_DESC bd = {};
    bd.ByteWidth = sizeof(Vertex) * vertexCount;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.CPUAccessFlags = 0;

    // 버텍스 버퍼 생성
    D3D11_SUBRESOURCE_DATA vbData = {};
    vbData.pSysMem = vertices.data();
    device->CreateBuffer(&bd, &vbData, &m_vertexBuffer);

    // 버텍스 버퍼 정보
    m_vertexCount = vertexCount;
    m_vertexBufferStride = sizeof(Vertex);
    m_vertexBufferOffset = 0;
}

void Mesh::CreateBoneVertexWeightBuffer(ID3D11Device* device, const vector<BoneVertexWeight>& vertices,
    UINT vertexCount)
{
    D3D11_BUFFER_DESC bd = {};
    bd.ByteWidth = sizeof(BoneVertexWeight) * vertexCount;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.CPUAccessFlags = 0;

    // 버텍스 버퍼 생성
    D3D11_SUBRESOURCE_DATA vbData = {};
    vbData.pSysMem = vertices.data();
    device->CreateBuffer(&bd, &vbData, &m_vertexBuffer);

    // 버텍스 버퍼 정보
    m_vertexCount = vertexCount;
    m_vertexBufferStride = sizeof(BoneVertexWeight);
    m_vertexBufferOffset = 0;
}

void Mesh::CreateIndexBuffer(ID3D11Device* device, const vector<UINT>& indices, UINT indexCount)
{
    D3D11_BUFFER_DESC bd = {};
    bd.ByteWidth = sizeof(UINT) * indexCount;
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.CPUAccessFlags = 0;

    // 인덱스 버퍼 생성
    D3D11_SUBRESOURCE_DATA ibData = {};
    ibData.pSysMem = indices.data();
    device->CreateBuffer(&bd, &ibData, &m_indexBuffer);

    // 인덱스 버퍼 정보
    m_indexCount = indexCount;
}

void Mesh::Create(ID3D11Device* device, aiMesh* mesh)
{
    m_materialIndex = mesh->mMaterialIndex;

    if (mesh->HasBones())
    {
        m_boneWeightVertices.resize(mesh->mNumVertices);

        for (UINT i = 0; i < mesh->mNumVertices; i++)
        {
            m_boneWeightVertices[i].Position = Vector3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
            m_boneWeightVertices[i].Texcoord = Vector2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
            m_boneWeightVertices[i].Normal = Vector3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
            m_boneWeightVertices[i].Tangent = Vector3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);
        }

        m_bones.resize(mesh->mNumBones);

        int boneIndexCounter = 0;
        map<string, int> BoneMapping;

        for (int i = 0; i < mesh->mNumBones; i++)
        {
            aiBone* aiBoneRef = mesh->mBones[i];
            string boneName = aiBoneRef->mName.C_Str();

            int boneIndex = 0;

            if (BoneMapping.find(boneName) == BoneMapping.end())
            {
                boneIndex = boneIndexCounter;
                boneIndexCounter++;

                m_bones[boneIndex] = new Bone();
                m_bones[boneIndex]->Create(aiBoneRef, i);

                BoneMapping[boneName] = boneIndex;
            }
            else
            {
                boneIndex = BoneMapping[boneName];
            }

            for (int j = 0; j < aiBoneRef->mNumWeights; j++)
            {
                unsigned int vertexID = aiBoneRef->mWeights[j].mVertexId;
                float weight = aiBoneRef->mWeights[j].mWeight;

                m_boneWeightVertices[vertexID].AddBoneData(boneIndex, weight);
            }
        }
        CreateBoneVertexWeightBuffer(device, m_boneWeightVertices, mesh->mNumVertices);
    }
    else
    {
        m_vertices.resize(mesh->mNumVertices);
        for (UINT i = 0; i < mesh->mNumVertices; i++)
        {
            m_vertices[i].Position = Vector3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
            m_vertices[i].Texcoord = Vector2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
            m_vertices[i].Normal = Vector3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
            m_vertices[i].Tangent = Vector3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);
        }
        CreateVertexBuffer(device, m_vertices, mesh->mNumVertices);
    }

    m_indices.resize(mesh->mNumFaces * 3);
    for (UINT i = 0; i < mesh->mNumFaces; i++)
    {
        m_indices[i * 3 + 0] = mesh->mFaces[i].mIndices[0];
        m_indices[i * 3 + 1] = mesh->mFaces[i].mIndices[1];
        m_indices[i * 3 + 2] = mesh->mFaces[i].mIndices[2];
    }
    CreateIndexBuffer(device, m_indices, mesh->mNumFaces * 3);
}

void Mesh::UpdateMatrixPalette(Matrix* matrixPalettePtr)
{
    assert(m_bones.size() < 128);
    for (int i = 0; i < m_bones.size(); i++)
    {
        matrixPalettePtr[i] = (m_bones[i]->m_offsetMatrix * *m_bones[i]->m_nodeWorldMatrixPtr).Transpose();
    }
}
