#pragma once
#include "SceneComponent.h"

#include <memory>

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

public:
    virtual void Update(float deltaTime);
    virtual void OnBeginPlay();
    virtual void OnEndPlay();

    bool ReadResource(std::string filePath);
    void SetResource(std::shared_ptr<SkeletalMeshResource> resource);
};

