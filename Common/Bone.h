#pragma once
#include "pch.h"
#include <string>

struct aiBone;

class Bone
{
public:
    Bone();
    ~Bone();

public:
    string m_BoneName;
    Matrix m_OffsetMatrix;
    Matrix* m_NodeWorldMatrixPtr;
    int m_BoneIndex;

public:
    void Create(aiBone* bone, int boneIndex);
};

