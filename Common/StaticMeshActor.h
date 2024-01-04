#pragma once
#include "Actor.h"

class StaticMeshComponent;

class StaticMeshActor :
    public Actor
{
public:
    StaticMeshActor();
    ~StaticMeshActor();

public:
    std::string m_filePath;
    StaticMeshComponent* m_staticMeshComponent;

    virtual void OnBeginPlay();
    virtual void OnEndPlay();
};

