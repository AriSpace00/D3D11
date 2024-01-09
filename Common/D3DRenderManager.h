#pragma once
#include <directxtk/SimpleMath.h>
using namespace DirectX::SimpleMath;

#include <list>

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

struct CB_Transform
{
    Matrix WorldMatrix;
    Matrix ViewMatrix;
    Matrix ProjectionMatrix;
};

struct CB_Material
{
    Vector4 Ambient = { 1.0f,1.0f,1.0f,1.0f };
    Vector4 Diffuse = { 1.0f,1.0f,1.0f,1.0f };
    Vector4 Specular = { 1.0f,1.0f,1.0f,1.0f };
    Vector4 Emissive = { 1.0f,1.0f,1.0f,1.0f };
    float Metalic;
    float Roughness;
    float SpecularPower = 80;
    float UseDiffuseMap;
    float UseNormalMap;
    float UseSpecularMap;
    float UseEmissiveMap;
    float UseOpacityMap;
    float UseMetalicMap;
    float UseRoughnessMap;
    Vector2 Material_pad0;
};

struct CB_MatrixPalette
{
    Matrix Array[128];
};

class Material;
class StaticMeshInstance;
class StaticMeshComponent;

class D3DRenderManager
{
public:
    D3DRenderManager();
    ~D3DRenderManager();

    static D3DRenderManager* m_instance;

public:
    HWND m_hWnd = nullptr;

    ID3D11Device* m_device = nullptr;
    ID3D11DeviceContext* m_deviceContext = nullptr;
    ID3D11RenderTargetView* m_renderTargetView = nullptr;
    ID3D11DepthStencilView* m_depthStencilView = nullptr;
    IDXGISwapChain* m_swapChain = nullptr;

    ID3D11VertexShader* m_staticMeshVS = nullptr;
    ID3D11InputLayout* m_staticMeshIL = nullptr;

    ID3D11PixelShader* m_pixelShader = nullptr;
    ID3D11SamplerState* m_samplerLinear = nullptr;
    ID3D11BlendState* m_alphaBlendState = nullptr;

    ID3D11Buffer* m_directionalLightCB = nullptr;
    ID3D11Buffer* m_transformCB = nullptr;
    ID3D11Buffer* m_materialCB = nullptr;
    ID3D11Buffer* m_matrixPaletteCB = nullptr;

    CB_DirectionalLight m_light;
    CB_Transform m_transform;
    CB_Material m_material;
    CB_MatrixPalette m_matrixPalette;

    UINT m_vertexBufferStride = 0;
    UINT m_vertexBufferOffset = 0;
    int m_indices = 0;

    DirectX::XMVECTOR m_eye;
    DirectX::XMVECTOR m_at;
    DirectX::XMVECTOR m_up;

    std::list<StaticMeshInstance*> m_staticMeshInstance;
    std::list<StaticMeshComponent*> m_staticMeshComponents;

    const float m_clearColor[4] = { 0.5f, 0.5f, 0.5f, 1.0f };

public:
    bool Initialize(HWND hWnd, UINT width, UINT height);
    void UnInitialize();
    void Update();
    void Render();

    bool InitImGUI();
    void UnInitImGUI();
    void RenderImGUI();

    void ApplyMaterial(Material* material);

private:
    void CreateStaticMesh_VS_IL();
    void CreatePS();

    void AddMeshInstance(StaticMeshComponent* staticMesh);

    void RenderStaticMeshInstance();
};