#pragma once
#include "SceneComponent.h"

class StaticMesh;

class StaticMeshComponent :
    public SceneComponent
{
public:
    StaticMeshComponent();
    ~StaticMeshComponent();

public:
    std::string m_filePath;
    std::shared_ptr<StaticMesh> m_resource;

    std::list<StaticMeshComponent*>::iterator m_iterator;

public:
    virtual void Update(float deltaTime);

    bool ReadResource(std::string filePath);
    void SetResource(std::shared_ptr<StaticMesh> resource);
};

