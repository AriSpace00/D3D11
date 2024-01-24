#pragma once
#include "Actor.h"

class EnvironmentMeshComponent;

class EnvironmentActor :
    public Actor
{
public:
    EnvironmentActor();
    ~EnvironmentActor();

public:
    EnvironmentMeshComponent* m_environmentMeshComponent = nullptr;

public:
    virtual void OnBeginPlay();
    virtual void OnEndPlay();

    EnvironmentMeshComponent* GetEnvironmentMeshComponent();
};

