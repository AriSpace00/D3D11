#pragma once
#include "SceneComponent.h"

#include <memory>

class StaticMeshInstance;
class StaticMeshResource;

class StaticMeshComponent :
    public SceneComponent
{
public:
    StaticMeshComponent();
    ~StaticMeshComponent();

public:
    std::string m_filePath;
    std::shared_ptr<StaticMeshResource> m_resource;
    std::vector<StaticMeshInstance> m_meshInstances;
    std::list<StaticMeshComponent*>::iterator m_iterator;

public:
    virtual void Update(float deltaTime);
    virtual void OnBeginPlay();
    virtual void OnEndPlay();

    bool ReadResource(std::string filePath);
    void SetResource(std::shared_ptr<StaticMeshResource> resource);
};

