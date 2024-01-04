#pragma once
#include <directxtk/SimpleMath.h>
using namespace DirectX::SimpleMath;

class Component;
class SceneComponent;
class World;

class Actor
{
    friend class World;

public:
    Actor();
    ~Actor();

private:
    std::list<std::shared_ptr<Component>> m_ownedComponents;

public:
    World* m_owner;
    std::list<std::shared_ptr<Actor>>::iterator m_worldIterator;
    SceneComponent* m_rootComponent;

public:
    virtual void Update(float deltaTime);
    virtual void Render(ID3D11DeviceContext* deviceContext);

    World* GetOwner();
    void SetOwner(World* world);

    void SetRootComponent(SceneComponent* rootComponent);
    SceneComponent* GetRootComponent() const;

    template<typename T>
    std::shared_ptr<T> CreateComponent(const std::string& name)
    {
        bool isBase = std::is_base_of<Component, T>::value;
        assert(isBase == true);
        std::shared_ptr<T> component = std::make_shared<T>();
        component->SetOwner(this);
        component->SetName(name);
        m_ownedComponents.push_back(component);
        return component;
    }

    virtual void OnBeginPlay();
    virtual void OnEndPlay();

    void SetWorldPosition(const Vector3& position);
    void SetWorldTransform(Matrix transform);
};

