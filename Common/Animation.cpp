#include "pch.h"
#include "Animation.h"

#include <assimp/scene.h>

void NodeAnimation::Create(aiNodeAnim* nodeAnim, double ticksPerSecond)
{
	assert(nodeAnim != nullptr);
	assert(nodeAnim->mNumPositionKeys == nodeAnim->mNumRotationKeys);
	assert(nodeAnim->mNumRotationKeys == nodeAnim->mNumScalingKeys);

	NodeName = nodeAnim->mNodeName.C_Str();
	AnimationKeys.resize(nodeAnim->mNumPositionKeys);

	// �ִϸ��̼� Ű�� ���� �� �ش� Ű ���� ������
	if (nodeAnim->mNumPositionKeys > 0)
	{
		for (int i = 0; i < nodeAnim->mNumPositionKeys; i++)
		{
			// ��ġ Ű������ ����
			AnimationKeys[i].Time = (nodeAnim->mPositionKeys[i].mTime / ticksPerSecond);
			AnimationKeys[i].Position =
				DirectX::XMFLOAT3(
					nodeAnim->mPositionKeys[i].mValue.x,
					nodeAnim->mPositionKeys[i].mValue.y,
					nodeAnim->mPositionKeys[i].mValue.z);

			// ȸ�� Ű������ ����
			AnimationKeys[i].Rotation =
				DirectX::XMFLOAT4(
					nodeAnim->mRotationKeys[i].mValue.x,
					nodeAnim->mRotationKeys[i].mValue.y,
					nodeAnim->mRotationKeys[i].mValue.z,
					nodeAnim->mRotationKeys[i].mValue.w);

			// ������ Ű������ ����
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
	/*int nextKeyIndex = (m_curKeyIndex + 1) % AnimationKeys.size();

	const AnimationKey* curKey = AnimationKeys[m_curKeyIndex];
	const AnimationKey* nextKey = AnimationKeys[nextKeyIndex];

	float interval = (curKey->Time - nextKey->Time) / m_animFps;
	float ratio = (m_duration - curKey->Time / m_animFps) / interval;

	Vector3 interpolationPosition = Vector3::Lerp(curKey->Position, nextKey->Position, ratio);
	Vector4 interpolationRotation = Quaternion::Slerp(curKey->Rotation, nextKey->Rotation, ratio);
	Vector3 interpolationScale = Vector3::Lerp(curKey->Scale, nextKey->Scale, ratio);

	Matrix interpolationTM = Matrix::CreateScale(interpolationScale) * Matrix::CreateFromQuaternion(interpolationRotation) * Matrix::CreateTranslation(interpolationPosition);

	m_interpolationTM = interpolationTM;*/
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
	// ��ü �ð����� = �����Ӽ� / 1�ʴ� �����Ӽ�
	m_duration = (float)(aiAnimation->mDuration / aiAnimation->mTicksPerSecond);
	for (int i = 0; i < aiAnimation->mNumChannels; i++)
	{
		NodeAnimation& refNodeAnim = m_nodeAnimations[i];
		aiNodeAnim* aiNodeAnim = aiAnimation->mChannels[i];
		refNodeAnim.Create(aiNodeAnim, aiAnimation->mTicksPerSecond);
	}
}