#include "pch.h"
#include "Animation.h"

#include <assimp/scene.h>

Animation::Animation()
{
}

Animation::~Animation()
{
}

void Animation::Create(const aiNodeAnim* nodeAnim)
{
    m_nodeName = nodeAnim->mNodeName.C_Str();
    m_animationKeys.resize(nodeAnim->mNumPositionKeys);

    // 애니메이션 키가 있을 때 해당 키 값을 가져옴
    if (nodeAnim->mNumPositionKeys > 0)
    {
        for (int i = 0; i < nodeAnim->mNumPositionKeys; i++)
        {
            m_animationKeys[i] = new AnimationKey();

            // 위치 키프레임 정보
            m_animationKeys[i]->Time = nodeAnim->mPositionKeys[i].mTime;
            m_animationKeys[i]->Position =
                DirectX::XMFLOAT3(
                    nodeAnim->mPositionKeys[i].mValue.x,
                    nodeAnim->mPositionKeys[i].mValue.y,
                    nodeAnim->mPositionKeys[i].mValue.z);

            // 회전 키프레임 정보
            m_animationKeys[i]->Rotation = 
                DirectX::XMFLOAT4(
                    nodeAnim->mRotationKeys[i].mValue.x,
                    nodeAnim->mRotationKeys[i].mValue.y,
                    nodeAnim->mRotationKeys[i].mValue.z,
                    nodeAnim->mRotationKeys[i].mValue.w);

            // 스케일 키프레임 정보
            m_animationKeys[i]->Scale = 
                DirectX::XMFLOAT3(
                    nodeAnim->mScalingKeys[i].mValue.x,
                    nodeAnim->mScalingKeys[i].mValue.y,
                    nodeAnim->mScalingKeys[i].mValue.z);
        }
    }
}

void Animation::Evaluate()
{
    int nextKeyIndex = (m_curKeyIndex + 1) % m_animationKeys.size();

    const AnimationKey* curKey = m_animationKeys[m_curKeyIndex];
    const AnimationKey* nextKey = m_animationKeys[nextKeyIndex];

    float interval = (curKey->Time - nextKey->Time) / m_animFps;
    float ratio = (m_duration - curKey->Time / m_animFps) / interval;

    Vector3 interpolationPosition = Vector3::Lerp(curKey->Position, nextKey->Position, ratio);
    Vector4 interpolationRotation = Quaternion::Slerp(curKey->Rotation, nextKey->Rotation, ratio);
    Vector3 interpolationScale = Vector3::Lerp(curKey->Scale, nextKey->Scale, ratio);

    Matrix interpolationTM = Matrix::CreateScale(interpolationScale) * Matrix::CreateFromQuaternion(interpolationRotation) * Matrix::CreateTranslation(interpolationPosition);

    m_interpolationTM = interpolationTM;
}

void Animation::Update(const float& deltaTime)
{
    m_duration += deltaTime;

    if (m_animationKeys.size() > 0)
    {
        m_nextKeyIndex = (m_curKeyIndex + 1) % m_animationKeys.size();

        if (m_duration > m_animationKeys[m_nextKeyIndex]->Time / m_animFps)
        {
            m_curKeyIndex = m_nextKeyIndex;

            if (m_curKeyIndex == 0)
            {
                m_duration = 0.f;
            }
        }
    }
}
