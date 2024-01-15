#pragma once
#include <assimp/scene.h>

struct BoneInfo
{
	BoneInfo() {}
	~BoneInfo() {}

	std::string Name;						// ���̸�
	Matrix RelativeTransform;				// �θ�κ����� ������� ��ȯ
	Matrix OffsetMatrix;					// �������� �޽��� ������� ��ġ(��ȯ)
	int NumChildren = 0;					// �ڽ��� ��
	int ParentBoneIndex = -1;

	void Set(const aiNode* pNode)
	{
		Name = std::string(pNode->mName.C_Str());
		RelativeTransform = Matrix(&pNode->mTransformation.a1).Transpose();
		NumChildren = pNode->mNumChildren;
	}
};

class Skeleton
{
public:
	Skeleton();
	~Skeleton();

public:
	std::string m_name;
	std::vector<BoneInfo> m_bones;
	std::map<std::string, int> m_boneMappingTable;	// Key: ��� �̸�, Value: m_bones�� �� �ε���

public:
	void Create(const aiScene* scene);
	void CountNode(int& count, const aiNode* node);

	BoneInfo* AddBoneInfo(const aiScene* scene, const aiNode* node);
	BoneInfo* FindBoneInfo(const std::string& name);
	BoneInfo* GetBoneInfo(int index);

	int GetBoneIndexByBoneName(const std::string& name);

	int GetBoneCount() { return (int)m_bones.size(); }
	const std::string& GetBoneName(int index) { return m_bones[index].Name; }
};

