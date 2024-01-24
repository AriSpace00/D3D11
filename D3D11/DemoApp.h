#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxtk/SimpleMath.h>

#include <string>
#include <vector>

#include "Common/App.h"
#include "Common/World.h"
#include "Common/Actor.h"

using namespace DirectX::SimpleMath;
using namespace DirectX;

class ImGUI;
class EnvironmentActor;

class DemoApp :
    public App
{
public:
    DemoApp(HINSTANCE hInstance);
    ~DemoApp();

public:
    World m_world;
    EnvironmentActor* m_environmentActor = nullptr;
	std::list<Actor*> m_spawnedActors;

public:
    virtual bool Initialize(UINT width, UINT height);
    virtual void Update();
    virtual void Render();

    virtual LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

    void LoadStaticMesh();
    void LoadSkeletalMesh();
    void DestroyMesh();
};

