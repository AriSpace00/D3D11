#pragma once

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

class D3DRenderManager
{
public:
    D3DRenderManager();
    ~D3DRenderManager();

    static D3DRenderManager* m_instance;

public:
    HWND m_hWnd;

    ID3D11Device* m_device = nullptr;
    ID3D11DeviceContext* m_deviceContext = nullptr;
    ID3D11RenderTargetView* m_renderTargetView = nullptr;
    ID3D11DepthStencilView* m_depthStencilView = nullptr;
    IDXGISwapChain* m_swapChain = nullptr;

    ID3D11VertexShader* m_vertexShader = nullptr;
    ID3D11PixelShader* m_pixelShader = nullptr;
    ID3D11InputLayout* m_inputLayout = nullptr;
    ID3D11SamplerState* m_samplerLinear = nullptr;

    ID3D11Buffer* m_directionalLightCB = nullptr;
    CB_DirectionalLight m_light;

    UINT m_vertexBufferStride = 0;
    UINT m_vertexBufferOffset = 0;

    Matrix m_viewMatrix;
    Matrix m_projectionMatrix;

    DirectX::XMVECTOR m_eye;
    DirectX::XMVECTOR m_at;
    DirectX::XMVECTOR m_up;

public:
    bool Initialize(HWND hWnd, UINT width, UINT height);
    void UnInitialize();
    void Update();
    void Render();

    bool InitImGUI();
    void UnInitImGUI();
};