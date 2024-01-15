#pragma once
#include <assimp/scene.h>

struct BoneInfo
{
	Matrix RelativeTransform;			//부모로부터의 상대적인 변환
	Matrix OffsetMatrix;				// 본에서의 메쉬의 상대적인 위치(변환)
	std::string Name;						// 본이름
	int NumChildren = 0;						// 자식의 수
	int ParentBoneIndex = -1;
	std::vector<std::string> MeshNames;		// 본에 연결된 메쉬들의 이름
	BoneInfo() {}
	void Set(const aiNode* pNode)
	{
		Name = std::string(pNode->mName.C_Str());
		RelativeTransform = Matrix(&pNode->mTransformation.a1).Transpose();
		NumChildren = pNode->mNumChildren;
	}
	~BoneInfo()
	{

	}
};

class Skeleton
{
public:
	std::string m_name;
	std::vector<BoneInfo> m_bones;
	std::map<std::string, int> m_boneMappingTable;

public:
	void Create(const aiScene* pScene);
	BoneInfo* AddBone(const aiScene* pScene, const aiNode* pNode);
	BoneInfo* FindBone(const std::string& name);
	BoneInfo* GetBone(int index);
	int GetBoneIndexByBoneName(const std::string& boneName);
	int GetBoneCount() { return (int)m_bones.size(); }
	const std::string& GetBoneName(int index) { return m_bones[index].Name; }
	void CountNode(int& Count, const aiNode* pNode);
};

