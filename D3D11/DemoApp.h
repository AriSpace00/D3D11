#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxtk/SimpleMath.h>

#include <string>
#include <vector>

#include "Common/App.h"

using namespace DirectX::SimpleMath;
using namespace DirectX;

class ImGUI;
//class Model;
//class Mesh;
class Material;

class DemoApp :
    public App
{
public:
    DemoApp(HINSTANCE hInstance);
    ~DemoApp();

public:

    float m_Roll = 0.0f;
    float m_Pitch = 0.0f;
    float m_Yaw = 0.0f;

    float m_CameraFovYRadius;
    float m_CameraNear;
    float m_CameraFar;

    //Model* m_Model;
    //std::vector<Mesh> m_Meshes;
    std::vector<Material> m_Materials;
    float m_MeshScale = 1.0f;
    std::wstring m_FBXFileName;

    LARGE_INTEGER m_Frequency;
    LARGE_INTEGER m_PrevTime, m_CurrentTime;
    double m_DeltaTime;

public:
    virtual bool Initialize(UINT width, UINT height);
    virtual void Update();
    virtual void Render();

    virtual LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};

