#pragma once
#include "Skeleton.h"
#include "Animation.h"

struct BoneWeightVertex
{
	Vector3 Position;		// ���� ��ġ ����.	
	Vector2 Texcoord;
	Vector3 Normal;
	Vector3 Tangent;

	// ����޴� ������ �ִ�4���� �����Ѵ�.
	int BlendIndices[4] = {};		// �����ϴ� ���� �ε����� ������ �ִ�128���� �ϴ�ó������
	float BlendWeights[4] = {};	// ����ġ ������ 1�� �Ǿ���Ѵ�.

	void AddBoneData(int boneIndex, float weight)
	{
		// ��� �ϳ��� �����Ͱ� ����־�� �Ѵ�.
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
	std::string NodeName;		 // ��� �̸�
	int BoneIndex = -1;			 // �� �ε���
	Matrix OffsetMatrix;		 // ������ �޽��� OffsetMatrix
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

	UINT m_vertexBufferStride = 0;    // ���ؽ� �ϳ��� ũ��
	UINT m_vertexBufferOffset = 0;    // ���ؽ� ������ ������
	UINT m_vertexCount = 0;           // ���ؽ� ���� 
	UINT m_indexCount = 0;            // �ε��� ����
	UINT m_materialIndex = 0;         // ���׸��� �ε���

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

