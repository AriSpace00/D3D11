#include "pch.h"
#include "SkeletalMeshActor.h"

#include "SkeletalMeshComponent.h"

SkeletalMeshActor::SkeletalMeshActor()
	: m_skeletalMeshComponent(nullptr)
{
	m_skeletalMeshComponent = CreateComponent<SkeletalMeshComponent>("SkeletalMeshComponent").get();
	SetRootComponent(m_skeletalMeshComponent);
}

SkeletalMeshActor::~SkeletalMeshActor()
{
}

void SkeletalMeshActor::OnBeginPlay()
{
	__super::OnBeginPlay();
}

void SkeletalMeshActor::OnEndPlay()
{
	__super::OnEndPlay();
}

SkeletalMeshComponent* SkeletalMeshActor::GetSkeletalMeshComponent()
{
	return m_skeletalMeshComponent;
}
