#pragma once
#include "SceneComponent.h"
#include "Bone.h"

#include <memory>

class Skeleton;
class SkeletalMeshInstance;
class SkeletalMeshResource;

class SkeletalMeshComponent :
    public SceneComponent
{
public:
    SkeletalMeshComponent();
    ~SkeletalMeshComponent();

public:
    std::string m_filePath;
    std::shared_ptr<SkeletalMeshResource> m_resource;
    std::vector<SkeletalMeshInstance> m_meshInstances;
    std::list<SkeletalMeshComponent*>::iterator m_iterator;

    Bone m_rootBone;
    float m_animationProgressTime = 0.0f;
    UINT m_animationIndex = 0;

public:
    virtual void Update(float deltaTime);
    virtual void OnBeginPlay();
    virtual void OnEndPlay();

    void CreateHierarchy(Skeleton* skeleton);
    void UpdateBoneAnimationReference(UINT index);
    void PlayAnimation(UINT index);

    bool ReadResource(std::string filePath);
    bool AddResource(std::string filePath);
    std::shared_ptr<SkeletalMeshResource> GetResource();
    void SetResource(std::shared_ptr<SkeletalMeshResource> resource);
};

