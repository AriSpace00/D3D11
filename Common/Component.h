#pragma once

class Actor;
class Component
{
public:
    Component();
    virtual ~Component();

protected:
    Actor* m_owner;
    std::string m_name;

public:
    void SetName(const std::string& name);
    const std::string& GetName() const;

    void SetOwner(Actor* owner);
    Actor* GetOwner();

    virtual void Update(float deltaTime) abstract;
    virtual void OnBeginPlay() abstract;
    virtual void OnEndPlay() abstract;
};

