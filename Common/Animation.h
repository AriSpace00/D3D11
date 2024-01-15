#pragma once

#include <string>
#include <vector>
#include <assimp/anim.h>

struct aiNodeAnim;

struct AnimationKey
{
    float Time = 0.0f;
    Vector3 Position;
    Quaternion Rotation;
    Vector3 Scale;
};

struct NodeAnimation
{
    string NodeName;
    vector<AnimationKey> AnimationKeys;

    void Create(aiNodeAnim* nodeAnim, double ticksPerSecond);
    void Evaluate(float time, Vector3& position, Quaternion& rotation, Vector3& scale);
};

class Animation
{
public:
    Animation();
    ~Animation();

public:
    std::string m_filePath;
    std::string m_name;
    float m_duration = 0.0f;    // 전체 길이
    vector<NodeAnimation> m_nodeAnimations;

public:
    void Create(const std::string filePath, const aiAnimation* aiAnimation);
};

