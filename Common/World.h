#pragma once

class Actor;
class World
{
public:
	World();
	~World();

public:
	bool m_isRun;
	std::list<std::shared_ptr<Actor>> m_gameObjects;

public:
	template<typename T>
	std::shared_ptr<T> CreateGameObject()
	{
		bool isBase = std::is_base_of<Actor, T>::value;
		assert(isBase == true);

		std::shared_ptr<T> newObject = std::make_shared<T>();
		newObject->SetOwner(this);
		m_gameObjects.push_back(newObject);
		newObject->m_worldIterator = --m_gameObjects.end();
		if (m_isRun)
			newObject->OnBeginPlay();

		return newObject();
	}

	void DestroyGameObject(Actor* gameObject);
	void Update(float deltaTime);
	void OnBeginPlay();
	void OnEndPlay();
};

