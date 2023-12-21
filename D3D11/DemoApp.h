#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxtk/SimpleMath.h>

#include <string>
#include <vector>

#include "Common/App.h"

using namespace DirectX::SimpleMath;
using namespace DirectX;

struct CB_DirectionalLight
{
    Vector3 Direction = { 0.0f,0.0f,1.0f };
    float DL_pad0;
    Vector4 Ambient = { 1.0f,1.0f,1.0f,1.0f };
    Vector4 Diffuse = { 1.0f,1.0f,1.0f,1.0f };
    Vector4 Specular = { 1.0f,1.0f,1.0f,1.0f };
    Vector3 EyePosition;
    float DL_pad1;
};

static_assert((sizeof(CB_DirectionalLight) % 16) == 0, "Constant Buffer size must be 16-byte aligned");

class ImGUI;
class Model;
class Mesh;
class Material;

class DemoApp :
    public App
{
public:
    DemoApp(HINSTANCE hInstance);
    ~DemoApp();

public:
    ImGUI* m_ImGUI;

    ID3D11Device* m_Device = nullptr;                       
    ID3D11DeviceContext* m_DeviceContext = nullptr;         
    ID3D11RenderTargetView* m_RenderTargetView = nullptr;   
    ID3D11DepthStencilView* m_DepthStencilView = nullptr;   
    IDXGISwapChain* m_SwapChain = nullptr;

    ID3D11VertexShader* m_VertexShader = nullptr;
    ID3D11PixelShader* m_PixelShader = nullptr;
    ID3D11InputLayout* m_InputLayout = nullptr;
    ID3D11SamplerState* m_SamplerLinear = nullptr;

    ID3D11Buffer* m_CBDirectionalLight = nullptr;
    CB_DirectionalLight m_Light;

    UINT m_VertexBufferStride = 0;
    UINT m_VertexBufferOffset = 0;

    Matrix m_ViewMatrix;
    Matrix m_ProjectionMatrix;

    XMVECTOR m_Eye;
    XMVECTOR m_At;
    XMVECTOR m_Up;

    float m_Roll = 0.0f;
    float m_Pitch = 0.0f;
    float m_Yaw = 0.0f;

    float m_CameraFovYRadius;
    float m_CameraNear;
    float m_CameraFar;

    Model* m_Model;
    std::vector<Mesh> m_Meshes;
    std::vector<Material> m_Materials;
    float m_MeshScale = 1.0f;
    std::wstring m_FBXFileName;

    const float m_ClearColor[4] = {0.5f, 0.5f, 0.5f, 1.0f};

    LARGE_INTEGER m_Frequency;
    LARGE_INTEGER m_PrevTime, m_CurrentTime;
    double m_DeltaTime;

public:
    virtual bool Initialize(UINT width, UINT height);
    virtual void Update();
    virtual void Render();

    bool InitD3D();
    void UnInitD3D();

    bool InitScene(); 
    void UnInitScene();

    void RunImGUI();

    virtual LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};

