#pragma once

class Component;
class SceneComponent;
class World;

class Actor
{
public:
    Actor();
    ~Actor();

private:
    std::list<std::shared_ptr<Component>> m_ownedComponents;

public:
    SceneComponent* m_rootComponent;
    World* m_ownerWorld;

    virtual void Update(float deltaTime);
    virtual void Render(ID3D11DeviceContext* deviceContext);

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
};

