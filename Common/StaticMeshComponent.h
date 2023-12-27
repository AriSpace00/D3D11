#pragma once
#include "SceneComponent.h"

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

    std::list<StaticMeshComponent*>::iterator m_iterator;

public:
    virtual void Update(float deltaTime);
};

