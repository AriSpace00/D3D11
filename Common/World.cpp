#include "pch.h"
#include "World.h"

#include "Actor.h"

World::World()
	: m_isRun(false)
{
}

World::~World()
{
}

void World::DestroyGameObject(Actor* gameObject)
{
	gameObject->OnEndPlay();
	m_gameObjects.erase(gameObject->m_worldIterator);
}

void World::Update(float deltaTime)
{
	for (auto& obj : m_gameObjects)
	{
		obj->Update(deltaTime);
	}
}

void World::OnBeginPlay()
{
	for (auto& obj : m_gameObjects)
	{
		obj->OnBeginPlay();
	}
	m_isRun = true;
}

void World::OnEndPlay()
{
	m_isRun = false;
	for (auto& obj : m_gameObjects)
	{
		obj->OnEndPlay();
	}
	m_gameObjects.clear();
}
