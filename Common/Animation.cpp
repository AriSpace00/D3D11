#include "pch.h"
#include "Helper.h"
#include "Animation.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

void NodeAnimation::Create(aiNodeAnim* nodeAnim, double ticksPerSecond)
{
	assert(nodeAnim != nullptr);
	assert(nodeAnim->mNumPositionKeys == nodeAnim->mNumRotationKeys);
	assert(nodeAnim->mNumRotationKeys == nodeAnim->mNumScalingKeys);

	NodeName = nodeAnim->mNodeName.C_Str();
	AnimationKeys.resize(nodeAnim->mNumPositionKeys);

	// 애니메이션 키가 있을 때 해당 키 값을 가져옴
	if (nodeAnim->mNumPositionKeys > 0)
	{
		for (int i = 0; i < nodeAnim->mNumPositionKeys; i++)
		{
			// 위치 키프레임 정보
			AnimationKeys[i].Time = (nodeAnim->mPositionKeys[i].mTime / ticksPerSecond);
			AnimationKeys[i].Position =
				DirectX::XMFLOAT3(
					nodeAnim->mPositionKeys[i].mValue.x,
					nodeAnim->mPositionKeys[i].mValue.y,
					nodeAnim->mPositionKeys[i].mValue.z);

			// 회전 키프레임 정보
			AnimationKeys[i].Rotation =
				DirectX::XMFLOAT4(
					nodeAnim->mRotationKeys[i].mValue.x,
					nodeAnim->mRotationKeys[i].mValue.y,
					nodeAnim->mRotationKeys[i].mValue.z,
					nodeAnim->mRotationKeys[i].mValue.w);

			// 스케일 키프레임 정보
			AnimationKeys[i].Scale =
				DirectX::XMFLOAT3(
					nodeAnim->mScalingKeys[i].mValue.x,
					nodeAnim->mScalingKeys[i].mValue.y,
					nodeAnim->mScalingKeys[i].mValue.z);
		}
	}
}

void NodeAnimation::Evaluate(float time, Vector3& position, Quaternion& rotation, Vector3& scale)
{
	assert(AnimationKeys.size() > 0);
	if (AnimationKeys.size() == 1)
	{
		position = AnimationKeys[0].Position;
		rotation = AnimationKeys[0].Rotation;
		scale = AnimationKeys[0].Scale;
	}
	else
	{
		int i = 0;
		for (i = 0; i < AnimationKeys.size(); i++)
		{
			if (AnimationKeys[i].Time > time)
				break;
		}

		float ratio = (time - AnimationKeys[i - 1].Time) / (AnimationKeys[i].Time - AnimationKeys[i - 1].Time);
		position = Vector3::Lerp(AnimationKeys[i - 1].Position, AnimationKeys[i].Position, ratio);
		rotation = Quaternion::Lerp(AnimationKeys[i - 1].Rotation, AnimationKeys[i].Rotation, ratio);
		scale = Vector3::Lerp(AnimationKeys[i - 1].Scale, AnimationKeys[i].Scale, ratio);
	}
}

Animation::Animation()
{
}

Animation::~Animation()
{
}

void Animation::Create(const std::string filePath, const aiAnimation* aiAnimation)
{
	m_filePath = filePath;
	m_name = aiAnimation->mName.C_Str();

	m_nodeAnimations.resize(aiAnimation->mNumChannels);
	// 전체 시간길이 = 프레임수 / 1초당 프레임수
	m_duration = (float)(aiAnimation->mDuration / aiAnimation->mTicksPerSecond);
	for (int i = 0; i < aiAnimation->mNumChannels; i++)
	{
		NodeAnimation& refNodeAnim = m_nodeAnimations[i];
		aiNodeAnim* aiNodeAnim = aiAnimation->mChannels[i];
		refNodeAnim.Create(aiNodeAnim, aiAnimation->mTicksPerSecond);
	}
}

void Animation::Create(const std::string filePath)
{
	std::filesystem::path path = ToWString(string(filePath));

	Assimp::Importer importer;

	importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, 0);	// $assimp_fbx$ 노드 생성안함
	unsigned int importFlags = aiProcess_ConvertToLeftHanded;						// 왼손 좌표계로 변환
	const aiScene* scene = importer.ReadFile(filePath, importFlags);
	if (!scene) {
		LOG_MESSAGEA("Error Loading Animation FBX File: %s", importer.GetErrorString());
		return;
	}
	assert(scene->mNumAnimations == 1);
	const aiAnimation* pAiAnimation = scene->mAnimations[0];
	assert(pAiAnimation->mNumChannels > 1); 
	Create(filePath, pAiAnimation);

	importer.FreeScene();
}
