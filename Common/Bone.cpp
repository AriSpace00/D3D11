#include "pch.h"
#include "Bone.h"

#include "Animation.h"

Bone::Bone()
	: m_nodeAnimation(nullptr)
	, m_animationTime(nullptr)
{
}

Bone::~Bone()
{
}

void Bone::Update(float deltaTime)
{
	if (m_nodeAnimation != nullptr)
	{
		Vector3 position, scale;
		Quaternion rotation;
		m_nodeAnimation->Evaluate(*m_animationTime, position, rotation, scale);
		Transform::m_localTM = Matrix::CreateScale(scale) * Matrix::CreateFromQuaternion(rotation) * Matrix::CreateTranslation(position);
	}

	Transform::Update(deltaTime);

	for (auto& child : m_children)
	{
		child.Update(deltaTime);
	}
}

Bone* Bone::FindBone(const std::string& name)
{
	if (m_name == name)
		return this;

	for (auto& child : m_children)
	{
		Bone* found = child.FindBone(name);
		if (found != nullptr)
			return found;
	}

	return nullptr;
}

Bone& Bone::CreateChild()
{
	return m_children.emplace_back();
}
