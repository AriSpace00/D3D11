#pragma once
#include "Component.h"
class SceneComponent :
    public Component
{
public:
    SceneComponent();
    virtual ~SceneComponent();

private:
    Matrix m_localTM;
    Matrix m_worldTM;

    Vector3 m_position;
    Vector3 m_rotation;
    Vector3 m_scale;

public:
    virtual void Update(float deltaTime);
};

