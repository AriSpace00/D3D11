#include "DemoApp.h"

#include <imgui.h>

#include "Common/World.h"
#include "Common/SkeletalMesh.h"
#include "Common/StaticMeshActor.h"
#include "Common/SkeletalMeshActor.h"
#include "Common/StaticMeshComponent.h"
#include "Common/SkeletalMeshComponent.h"
#include "Common/EnvironmentActor.h"
#include "Common/EnvironmentMeshComponent.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

DemoApp::DemoApp(HINSTANCE hInstance)
	: App(hInstance)
{
}

DemoApp::~DemoApp()
{
	DestroyMesh();
}

bool DemoApp::Initialize(UINT width, UINT height)
{
	App::Initialize(width, height);

	/// IBL
	/*std::string envCube = "../Resource/IBL_Test/EnvironmentCube.fbx";
	std::wstring envHDR = L"../Resource/IBL_Test/BakerSample/BakerSampleEnvHDR.dds";
	std::wstring envDiffuseHDR = L"../Resource/IBL_Test/BakerSample/BakerSampleDiffuseHDR.dds";
	std::wstring envSpecularHDR = L"../Resource/IBL_Test/BakerSample/BakerSampleSpecularHDR.dds";
	std::wstring envBRDFHDR = L"../Resource/IBL_Test/BakerSample/BakerSampleBrdf.dds";
	
	m_environmentActor = m_world.CreateGameObject<EnvironmentActor>().get();
	EnvironmentMeshComponent* envComponent = m_environmentActor->GetEnvironmentMeshComponent();
	envComponent->ReadEnvironmentMesh(envCube);
	envComponent->ReadEnvironmentTexture(envHDR);
	envComponent->ReadEnvironmentDiffuseTexture(envDiffuseHDR);
	envComponent->ReadEnvironmentSpecularTexture(envSpecularHDR);
	envComponent->ReadEnvironmentBRDFTexture(envBRDFHDR);
	m_environmentActor->SetWorldPosition(Vector3(0.f, 0.f, 0.f));
	D3DRenderManager::m_instance->SetEnvironment(m_environmentActor);*/

	/// World
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

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT DemoApp::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
		return true;

	switch (message)
	{
	case WM_KEYDOWN:
		if (wParam == VK_UP)
		{
			LoadStaticMesh();
		}
		if (wParam == VK_DOWN)
		{
			DestroyMesh();
		}
		if (wParam == VK_LEFT)
		{
			LoadSkeletalMesh();
		}
		break;
	}
	return __super::WndProc(hWnd, message, wParam, lParam);
}

void DemoApp::LoadStaticMesh()
{
	auto stActor = m_world.CreateGameObject<StaticMeshActor>();
	StaticMeshComponent* stComponent = stActor->GetStaticMeshComponent();
	stComponent->ReadResource("../Resource/FBXLoad_Test/fbx/cerberus2.fbx");

	int range = 500;
	float posx = (float)(rand() % range) - range * 0.5f;
	stActor->SetWorldPosition(Vector3(posx, 300, 0));

	m_spawnedActors.push_back(stActor.get());

	auto stActor1 = m_world.CreateGameObject<StaticMeshActor>();
	StaticMeshComponent* stComponent1 = stActor1->GetStaticMeshComponent();
	stComponent1->ReadResource("../Resource/FBXLoad_Test/fbx/zeldaPosed001.fbx");

	int range1 = 500;
	float posx1 = (float)(rand() % range1) - range1 * 0.5f;
	stActor1->SetWorldPosition(Vector3(posx1, 300, 0));

	m_spawnedActors.push_back(stActor1.get());
}

void DemoApp::LoadSkeletalMesh()
{
	auto skActor = m_world.CreateGameObject<SkeletalMeshActor>();
	SkeletalMeshComponent* skComponent = skActor->GetSkeletalMeshComponent();
	skComponent->ReadResource("../Resource/FBXLoad_Test/fbx/Character.fbx");
	skComponent->AddResource("../Resource/FBXLoad_Test/fbx/Character_Run.fbx");
	skComponent->AddResource("../Resource/FBXLoad_Test/fbx/SkinningTest.fbx");

	int range = 500;
	float posx = (float)(rand() % range) - range * 0.3f;
	skActor->SetWorldPosition(Vector3(posx, 300, 0));

	auto skResource = skComponent->GetResource();
	int index = rand() % skResource->m_animations.size();
	skComponent->PlayAnimation(index);

	m_spawnedActors.push_back(skActor.get());
}

void DemoApp::DestroyMesh()
{
	auto it = m_spawnedActors.begin();
	if (it == m_spawnedActors.end())
		return;

	m_world.DestroyGameObject(*it);
	m_spawnedActors.erase(it);
}

