#pragma once

#include <d3d11.h>
#include <directxtk/SimpleMath.h>

#include "Common/App.h"

using namespace DirectX::SimpleMath;
using namespace DirectX;

class ImGUI;

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

    UINT m_VertexBufferStride = 0;
    UINT m_VertexBufferOffset = 0;

    Matrix m_ViewMatrix;
    Matrix m_ProjectionMatrix;

    Vector4 m_ClearColor;

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

