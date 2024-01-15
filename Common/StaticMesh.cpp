#include "pch.h"
#include "StaticMesh.h"
#include "Material.h"
#include "D3DRenderManager.h"

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

void StaticMesh::CreateIndexBuffer(ID3D11Device* device, const vector<UINT>& indices, UINT indexCount)
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

void StaticMesh::Create(ID3D11Device* device, aiMesh* mesh)
{
	m_materialIndex = mesh->mMaterialIndex;

	m_vertices.resize(mesh->mNumVertices);
	for (UINT i = 0; i < mesh->mNumVertices; i++)
	{
		m_vertices[i].Position = Vector3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
		m_vertices[i].Texcoord = Vector2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
		m_vertices[i].Normal = Vector3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
		m_vertices[i].Tangent = Vector3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);
	}
	CreateVertexBuffer(device, m_vertices, mesh->mNumVertices);

	m_indices.resize(mesh->mNumFaces * 3);
	for (UINT i = 0; i < mesh->mNumFaces; i++)
	{
		m_indices[i * 3 + 0] = mesh->mFaces[i].mIndices[0];
		m_indices[i * 3 + 1] = mesh->mFaces[i].mIndices[1];
		m_indices[i * 3 + 2] = mesh->mFaces[i].mIndices[2];
	}
	CreateIndexBuffer(device, m_indices, mesh->mNumFaces * 3);
}

StaticMeshResource::StaticMeshResource()
{
}

StaticMeshResource::~StaticMeshResource()
{
}

void StaticMeshResource::Create(const std::string& path)
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
		m_meshes[i].Create(D3DRenderManager::m_instance->m_device, scene->mMeshes[i]);
	}

	m_materials.resize(scene->mNumMaterials);
	for (unsigned int i = 0; i < scene->mNumMaterials; ++i)
	{
		m_materials[i].SetFileName(m_fileName);
		m_materials[i].Create(scene->mMaterials[i]);
	}

	importer.FreeScene();
}

Material* StaticMeshResource::GetMeshMaterial(UINT index)
{
	assert(index < m_meshes.size());
	UINT mindex = m_meshes[index].m_materialIndex;
	assert(mindex < m_materials.size());
	return &m_materials[mindex];
}
