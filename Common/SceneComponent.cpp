#include "pch.h"
#include "SceneComponent.h"

SceneComponent::SceneComponent()
	: m_parent(nullptr)
	, m_localTM(Matrix::Identity)
	, m_worldTM(Matrix::Identity)
	, m_localPosition(Vector3(0.f, 0.f, 0.f))
	, m_localRotation(Vector3(0.f, 0.f, 0.f))
	, m_localScale(Vector3(1.f, 1.f, 1.f))
{
}

SceneComponent::~SceneComponent()
{
}

void SceneComponent::Update(float deltaTime)
{
	__super::Update(deltaTime);

	m_localTM = Matrix::CreateScale(m_localScale) *
		Matrix::CreateFromYawPitchRoll(m_localRotation) *
		Matrix::CreateTranslation(m_localPosition);

	if (m_parent)
	{
		m_worldTM = m_parent->m_worldTM * m_localTM;
	}
	else
	{
		m_worldTM = m_localTM;
	}
}

void SceneComponent::OnBeginPlay()
{
}

void SceneComponent::OnEndPlay()
{
}

void SceneComponent::SetParent(SceneComponent* parent)
{
	m_parent = parent;
}

void SceneComponent::SetLocalPosition(const Vector3& position)
{
	m_localPosition = position;
}

void SceneComponent::SetLocalRotation(const Vector3& rotation)
{
	m_localRotation = rotation;
}

void SceneComponent::SetLocalScale(const Vector3& scale)
{
	m_localScale = scale;
}

void SceneComponent::SetLocalTransform(Matrix transform)
{
	Quaternion quaternion;
	transform.Decompose(m_localScale, quaternion, m_localPosition);
	m_localRotation = quaternion.ToEuler();
}

