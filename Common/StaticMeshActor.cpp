#include "pch.h"
#include "StaticMeshActor.h"

#include "StaticMeshComponent.h"

StaticMeshActor::StaticMeshActor()
    : m_staticMeshComponent(nullptr)
{
    m_staticMeshComponent = CreateComponent<StaticMeshComponent>("StaticMeshComponent").get();
    SetRootComponent(m_staticMeshComponent);
}

StaticMeshActor::~StaticMeshActor()
{
}
