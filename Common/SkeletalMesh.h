#pragma once
#include "Skeleton.h"
#include "Animation.h"

struct BoneWeightVertex
{
	Vector3 Position;		// 정점 위치 정보.	
	Vector2 Texcoord;
	Vector3 Normal;
	Vector3 Tangent;

	// 영향받는 본수는 최대4개로 제한한다.
	int BlendIndices[4] = {};		// 참조하는 본의 인덱스의 범위는 최대128개로 일단처리하자
	float BlendWeights[4] = {};	// 가중치 총합은 1이 되어야한다.

	void AddBoneData(int boneIndex, float weight)
	{
		// 적어도 하나는 데이터가 비어있어야 한다.
		assert(BlendWeights[0] == 0.0f || BlendWeights[1] == 0.0f ||
			BlendWeights[2] == 0.0f || BlendWeights[3] == 0.0f);
		for (int i = 0; i < 4; i++)
		{
			if (BlendWeights[i] == 0.0f)
			{
				BlendIndices[i] = boneIndex;
				BlendWeights[i] = weight;
				return;
			}
		}
	}
};

struct BoneReference
{
	std::string NodeName;		 // 노드 이름
	int BoneIndex = -1;			 // 본 인덱스
	Matrix OffsetMatrix;		 // 본기준 메시의 OffsetMatrix
};

struct aiMesh;
class Material;

class SkeletalMesh
{
public:
	SkeletalMesh();
	~SkeletalMesh();

public:
	ID3D11Buffer* m_vertexBuffer;
	ID3D11Buffer* m_indexBuffer;

	UINT m_vertexBufferStride = 0;    // 버텍스 하나의 크기
	UINT m_vertexBufferOffset = 0;    // 버텍스 버퍼의 오프셋
	UINT m_vertexCount = 0;           // 버텍스 개수 
	UINT m_indexCount = 0;            // 인덱스 개수
	UINT m_materialIndex = 0;         // 머테리얼 인덱스

	std::vector<BoneWeightVertex> m_vertices;
	std::vector<BoneReference> m_boneReferences;
	std::vector<UINT> m_indices;

private:
	void CreateVertexBuffer(ID3D11Device* device, const std::vector<BoneWeightVertex>& vertices, UINT vertexCount);
	void CreateIndexBuffer(ID3D11Device* device, const std::vector<UINT>& indices, UINT indexCount);

public:
	void Create(aiMesh* mesh, Skeleton* skeleton);
};

class SkeletalMeshResource
{
public:
	SkeletalMeshResource();
	~SkeletalMeshResource();

public:
	std::wstring m_fileName;
	std::vector<SkeletalMesh> m_meshes;
	std::vector<Material> m_materials;
	Skeleton m_skeleton;

	std::vector<std::shared_ptr<Animation>> m_animations;

public:
	void Create(const std::string& path);
	Material* GetMeshMaterial(UINT index);
};

