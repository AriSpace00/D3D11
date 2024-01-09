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

		return newObject;
	}

	void DestroyGameObject(Actor* gameObject);
	void Update(float deltaTime);

	// App::ChangeWorld() 에서 호출
	// OnBeginPlay 해주는 시점에 world 내의 모든 리소스 읽어오려고 있는듯
	// 지금은 DemoApp::Initalize() 에서 component 통해 리소스를 읽어오기 때문에 필요없을 것 같음 
	void OnBeginPlay(); 
	void OnEndPlay();
};

