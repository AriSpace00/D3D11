#pragma once

#include <string>
#include <vector>

struct aiNodeAnim;

struct AnimationKey
{
    float Time = 0.0f;
    Vector3 Position;
    Quaternion Rotation;
    Vector3 Scale;
};

class Animation
{
public:
    Animation();
    ~Animation();

public:
    int m_CurKeyIndex = 0;
    int m_NextKeyIndex = 0;
    int m_AnimFps = 30;
    float m_Duration = 0.0f;

    string m_NodeName;
    vector<AnimationKey*> m_AnimationKeys;

    Matrix m_InterpolationTM;

public:
    void Create(const aiNodeAnim* nodeAnim);
    void Evaluate();
    void Update(const float& deltaTime);
};

