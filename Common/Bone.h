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
    string m_boneName;
    Matrix m_offsetMatrix;
    Matrix* m_nodeWorldMatrixPtr;
    int m_boneIndex;

public:
    void Create(aiBone* bone, int boneIndex);
};

