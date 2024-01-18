#pragma once
#include "Transform.h"

struct NodeAnimation;

class Bone :
	public Transform
{
public:
    Bone();
    ~Bone();

public:
    std::string m_name;
    std::vector<Bone> m_children;
    NodeAnimation* m_nodeAnimation; // 노드가 사용할 NodeAnimation 주소
    float* m_animationTime;         // 현재 노드가 애니메이션에서 어느 시간에 있는지

public:
    virtual void Update(float deltaTime);
    void SetAnimationTimePtr(float* val) { m_animationTime = val; }
	Bone* FindBone(const std::string& name);
    Bone& CreateChild();
};

