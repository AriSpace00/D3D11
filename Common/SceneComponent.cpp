#include "pch.h"
#include "SceneComponent.h"

SceneComponent::SceneComponent()
	: m_localPosition(Vector3(0.f, 0.f, 0.f))
	, m_localRotation(Vector3(0.f, 0.f, 0.f))
	, m_localScale(Vector3(1.f, 1.f, 1.f))
{
}

SceneComponent::~SceneComponent()
{
}

void SceneComponent::Update(float deltaTime)
{
	Transform::m_localTM = Matrix::CreateScale(m_localScale) *
		Matrix::CreateFromYawPitchRoll(m_localRotation) *
		Matrix::CreateTranslation(m_localPosition);

	Transform::Update(deltaTime);
}

void SceneComponent::OnBeginPlay()
{
}

void SceneComponent::OnEndPlay()
{
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

