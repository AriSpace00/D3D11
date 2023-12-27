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
    int m_curKeyIndex = 0;
    int m_nextKeyIndex = 0;
    int m_animFps = 30;
    float m_duration = 0.0f;

    string m_nodeName;
    vector<AnimationKey*> m_animationKeys;

    Matrix m_interpolationTM;

public:
    void Create(const aiNodeAnim* nodeAnim);
    void Evaluate();
    void Update(const float& deltaTime);
};

