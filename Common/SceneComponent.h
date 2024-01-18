#pragma once
#include "Component.h"
#include "Transform.h"

class SceneComponent :
    public Component, public Transform
{
public:
    SceneComponent();
    virtual ~SceneComponent();

protected:
    Vector3 m_localPosition;
    Vector3 m_localRotation;
    Vector3 m_localScale;

public:
    virtual void Update(float deltaTime);
    virtual void OnBeginPlay();
    virtual void OnEndPlay();

    void SetLocalPosition(const Vector3& position);
    void SetLocalRotation(const Vector3& rotation);
    void SetLocalScale(const Vector3& scale);
    void SetLocalTransform(Matrix transform);
};

