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
    m_boneName = bone->mName.C_Str();
    m_offsetMatrix = Matrix(&bone->mOffsetMatrix.a1).Transpose();
    m_boneIndex = boneIndex;
}
