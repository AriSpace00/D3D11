#include "pch.h"
#include "D3DRenderManager.h"
#include "ResourceManager.h"
#include "SkeletalMesh.h"
#include "SkeletalMeshInstance.h"
#include "SkeletalMeshComponent.h"

SkeletalMeshComponent::SkeletalMeshComponent()
{
	D3DRenderManager::m_instance->m_skeletalMeshComponents.push_back(this);
	m_iterator = --D3DRenderManager::m_instance->m_skeletalMeshComponents.end();
}

SkeletalMeshComponent::~SkeletalMeshComponent()
{
	D3DRenderManager::m_instance->m_skeletalMeshComponents.erase(m_iterator);
}

void SkeletalMeshComponent::Update(float deltaTime)
{
	__super::Update(deltaTime);

	if (!m_resource->m_animations.empty())
	{
		m_animationProgressTime += deltaTime;
		// fmod(x, y) : x/y 의 나머지를 계산해 float로 반환
		m_animationProgressTime = fmod(m_animationProgressTime, m_resource->m_animations[m_animationIndex]->m_duration);
	}

	m_rootBone.Update(deltaTime);
}

void SkeletalMeshComponent::OnBeginPlay()
{
	__super::OnBeginPlay();
}

void SkeletalMeshComponent::OnEndPlay()
{
	__super::OnEndPlay();
}

void SkeletalMeshComponent::CreateHierarchy(Skeleton* skeleton)
{
	BoneInfo* rootBone = skeleton->GetBoneInfo(0);
	m_rootBone.m_name = rootBone->Name;
	m_rootBone.m_children.reserve(rootBone->NumChildren);
	m_rootBone.SetParent(this);

	for (int i = 1; i < skeleton->GetBoneCount(); i++)
	{
		BoneInfo* boneInfo = skeleton->GetBoneInfo(i);
		assert(boneInfo != nullptr);
		assert(boneInfo->ParentBoneIndex != -1);

		Bone* parentBone = m_rootBone.FindBone(skeleton->GetBoneName(boneInfo->ParentBoneIndex));
		assert(parentBone != nullptr);

		auto& childBone = parentBone->CreateChild();
		childBone.m_name = boneInfo->Name;
		childBone.m_localTM = boneInfo->RelativeTransform;
		childBone.m_parent = parentBone;
		childBone.m_animationTime = &m_animationProgressTime;
		childBone.m_children.reserve(boneInfo->NumChildren);
	}
}

void SkeletalMeshComponent::UpdateBoneAnimationReference(UINT index)
{
	assert(index < m_resource->m_animations.size());
	auto animation = m_resource->m_animations[index];
	for (int i = 0; i < animation->m_nodeAnimations.size(); i++)
	{
		NodeAnimation& nodeAnimation = animation->m_nodeAnimations[i];
		Bone* bone = m_rootBone.FindBone(nodeAnimation.NodeName);
		assert(bone != nullptr);
		bone->m_nodeAnimation = &animation->m_nodeAnimations[i];
	}
}

void SkeletalMeshComponent::PlayAnimation(UINT index)
{
	assert(index < m_resource->m_animations.size());
	m_animationIndex = index;
	m_animationProgressTime = 0.0f;
	UpdateBoneAnimationReference(index);
}

bool SkeletalMeshComponent::ReadResource(std::string filePath)
{
	std::shared_ptr<SkeletalMeshResource> resourcePtr = ResourceManager::m_instance->CreateSkeletalMeshResource(filePath);
	if (resourcePtr == nullptr)
	{
		return false;
	}
	SetResource(resourcePtr);
	return true;
}

bool SkeletalMeshComponent::AddResource(std::string filePath)
{
	assert(m_resource);
	auto animation = ResourceManager::m_instance->CreateAnimation(filePath);
	if (!animation)
		return false;

	m_resource->m_animations.push_back(animation);
	return true;
}

std::shared_ptr<SkeletalMeshResource> SkeletalMeshComponent::GetResource()
{
	return m_resource;
}

void SkeletalMeshComponent::SetResource(std::shared_ptr<SkeletalMeshResource> resource)
{
	assert(resource);
	m_resource = resource;

	CreateHierarchy(&resource->m_skeleton);

	m_meshInstances.resize(m_resource->m_meshes.size());
	for (int i = 0; i < m_resource->m_meshes.size(); i++)
	{
		m_meshInstances[i].Create(&m_resource->m_meshes[i], m_resource->GetMeshMaterial(i), &m_rootBone);
	}

	UpdateBoneAnimationReference(0);
}
