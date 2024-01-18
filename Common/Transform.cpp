#include "pch.h"
#include "Transform.h"

Transform::Transform()
	: m_localTM(Matrix::Identity)
	, m_worldTM(Matrix::Identity)
	, m_parent(nullptr)
{
}

Transform::~Transform()
{
}

void Transform::SetParent(Transform* parent)
{
	m_parent = parent;
}

void Transform::Update(float deltaTime)
{
	if (m_parent)
	{
		m_worldTM = m_localTM * m_parent->m_worldTM;
	}
	else
	{
		m_worldTM = m_localTM;
	}
}
