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
    NodeAnimation* m_nodeAnimation; // ��尡 ����� NodeAnimation �ּ�
    float* m_animationTime;         // ���� ��尡 �ִϸ��̼ǿ��� ��� �ð��� �ִ���

public:
    virtual void Update(float deltaTime);
    void SetAnimationTimePtr(float* val) { m_animationTime = val; }
	Bone* FindBone(const std::string& name);
    Bone& CreateChild();
};

