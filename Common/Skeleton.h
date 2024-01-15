#pragma once
#include <assimp/scene.h>

struct BoneInfo
{
	BoneInfo() {}
	~BoneInfo() {}

	std::string Name;						// 본이름
	Matrix RelativeTransform;				// 부모로부터의 상대적인 변환
	Matrix OffsetMatrix;					// 본에서의 메쉬의 상대적인 위치(변환)
	int NumChildren = 0;					// 자식의 수
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
	std::map<std::string, int> m_boneMappingTable;	// Key: 노드 이름, Value: m_bones의 본 인덱스

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

