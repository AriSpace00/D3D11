#include "pch.h"
#include "Bone.h"

#include <assimp/mesh.h>

Bone::Bone()
{
}

Bone::~Bone()
{
}

void Bone::Create(aiBone* bone, int boneIndex)
{
    m_BoneName = bone->mName.C_Str();
    m_OffsetMatrix = Matrix(&bone->mOffsetMatrix.a1).Transpose();
    m_BoneIndex = boneIndex;
}
