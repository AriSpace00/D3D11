#include "pch.h"
#include "Skeleton.h"

Skeleton::Skeleton()
{
}

Skeleton::~Skeleton()
{
}

void Skeleton::Create(const aiScene* scene)
{
	m_name = scene->mName.C_Str();

	// key: ��� �̸�, Value: ��� ������ boneMappingTable�� ��Ƶд�.
	int numNode = 0;
	CountNode(numNode, scene->mRootNode);

	// ���� �׿� �ش��ϴ� �� �̸��� ����.
	// ���� �̸��� ���� key��(��� �̸�)�� �� �ε����� �־��ش�.
	m_bones.reserve(numNode);
	AddBoneInfo(scene, scene->mRootNode);

	// boneMappingTable���� �ش��ϴ� ���� ã�� OffsetMatrix�� �����Ѵ�.
	for (UINT i = 0; i < scene->mNumMeshes; i++)
	{
		aiMesh* aiMesh = scene->mMeshes[i];
		if (aiMesh->HasBones())
			continue;

		for (UINT j = 0; j < aiMesh->mNumBones; j++)
		{
			auto aiBone = aiMesh->mBones[j];
			BoneInfo* boneInfo = FindBoneInfo(aiBone->mName.C_Str());
			boneInfo->OffsetMatrix = Matrix(&aiBone->mOffsetMatrix.a1).Transpose();
		}
	}
}

BoneInfo* Skeleton::AddBoneInfo(const aiScene* scene, const aiNode* node)
{
	BoneInfo& bone = m_bones.emplace_back();
	bone.Set(node);

	int boneIndex = (int)(m_bones.size() - 1);
	m_boneMappingTable[bone.Name] = boneIndex;

	UINT numChild = node->mNumChildren;
	for (UINT i = 0; i < numChild; i++)
	{
		BoneInfo* child = AddBoneInfo(scene, node->mChildren[i]);
		child->ParentBoneIndex = boneIndex;
	}

	return &m_bones[boneIndex];
}

BoneInfo* Skeleton::FindBoneInfo(const std::string& name)
{
	auto iter = m_boneMappingTable.find(name);
	if (iter == m_boneMappingTable.end())
		return nullptr;
	return &m_bones[iter->second];
}

BoneInfo* Skeleton::GetBoneInfo(int index)
{
	if (index < 0 || index >= m_bones.size())
		return nullptr;
	return &m_bones[index];
}

int Skeleton::GetBoneIndexByBoneName(const std::string& name)
{
	auto iter = m_boneMappingTable.find(name);
	if (iter == m_boneMappingTable.end())
		return -1;
	return iter->second;
}

void Skeleton::CountNode(int& count, const aiNode* node)
{
	count++;
	std::string nodeName = node->mName.C_Str();
	m_boneMappingTable[nodeName] = count;

	for (UINT i = 0; i < node->mNumChildren; i++)
	{
		CountNode(count, node->mChildren[i]);
	}
}
