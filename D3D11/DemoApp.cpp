#include "DemoApp.h"

#include "Common/World.h"
#include "Common/StaticMeshActor.h"
#include "Common/StaticMeshComponent.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

DemoApp::DemoApp(HINSTANCE hInstance)
	: App(hInstance)
{
}

DemoApp::~DemoApp()
{
	DestroyStaticMesh();
}

bool DemoApp::Initialize(UINT width, UINT height)
{
	App::Initialize(width, height);

	LoadStaticMesh();
	ChangeWorld(&m_world);

	return true;
}

void DemoApp::Update()
{
	App::Update();
}

void DemoApp::Render()
{
	App::Render();
}

LRESULT DemoApp::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return __super::WndProc(hWnd, message, wParam, lParam);
}

void DemoApp::LoadStaticMesh()
{
	auto stActor = m_world.CreateGameObject<StaticMeshActor>();
	StaticMeshComponent* stComponent = stActor->GetStaticMeshComponent();
	stComponent->ReadResource("../Resource/FBXLoad_Test/fbx/cerberus2.fbx");
	stActor->SetWorldPosition(Vector3(0,300,0));
	m_spawnedActors.push_back(stActor.get());
}

void DemoApp::DestroyStaticMesh()
{
	auto it = m_spawnedActors.begin();
	if (it == m_spawnedActors.end())
		return;

	m_world.DestroyGameObject(*it);
	m_spawnedActors.erase(it);
}
