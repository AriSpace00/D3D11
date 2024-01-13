#pragma once
#include "Actor.h"

class SkeletalMeshComponent;

class SkeletalMeshActor :
    public Actor
{
public:
    SkeletalMeshActor();
    ~SkeletalMeshActor();

public:
    std::string m_filePath;
    SkeletalMeshComponent* m_skeletalMeshComponent;

public:
    virtual void OnBeginPlay();
    virtual void OnEndPlay();

    SkeletalMeshComponent* GetSkeletalMeshComponent();
};

