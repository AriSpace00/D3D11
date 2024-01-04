#pragma once
#include "Component.h"
class SceneComponent :
    public Component
{
public:
    SceneComponent();
    virtual ~SceneComponent();

public:
    SceneComponent* m_parent;

private:
    Matrix m_localTM;
    Matrix m_worldTM;

    Vector3 m_localPosition;
    Vector3 m_localRotation;
    Vector3 m_localScale;

public:
    virtual void Update(float deltaTime);
    virtual void OnBeginPlay();
    virtual void OnEndPlay();

    void SetParent(SceneComponent* parent);

    void SetLocalPosition(const Vector3& position);
    void SetLocalRotation(const Vector3& rotation);
    void SetLocalScale(const Vector3& scale);
    void SetLocalTransform(Matrix transform);
};

