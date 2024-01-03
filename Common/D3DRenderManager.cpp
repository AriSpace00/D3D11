#include "pch.h"
#include "D3DRenderManager.h"
#include "Helper.h"

#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>


D3DRenderManager* D3DRenderManager::m_instance = nullptr;

D3DRenderManager::D3DRenderManager()
{
    assert(m_instance == nullptr);
    m_instance = this;
}

D3DRenderManager::~D3DRenderManager()
{
    UnInitialize();
}

bool D3DRenderManager::Initialize(HWND hWnd, UINT width, UINT height)
{
    m_hWnd = hWnd;

    // �����
    HRESULT hr = 0;

    // ����ü�� �Ӽ� ���� ����ü ����
    DXGI_SWAP_CHAIN_DESC swapDesc = {};

    swapDesc.BufferCount = 1;
    swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapDesc.OutputWindow = m_hWnd;                         // ����ü�� ����� â �ڵ� ��
    swapDesc.Windowed = true;                               // â ��� ���� ����
    swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

    // �� ����(�ؽ�ó)�� ����/���� ũ�� ����
    swapDesc.BufferDesc.Width = width;
    swapDesc.BufferDesc.Height = height;

    // ȭ�� �ֻ��� ����
    swapDesc.BufferDesc.RefreshRate.Numerator = 60;
    swapDesc.BufferDesc.RefreshRate.Denominator = 1;

    // ���ø� ���� ����
    swapDesc.SampleDesc.Count = 1;
    swapDesc.SampleDesc.Quality = 0;

    UINT creationFlags = 0;

#ifdef _DEBUG
    creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
    // 1. ��ġ ����, 2. ����ü�� ����, 3. ��ġ ���ؽ�Ʈ ����
    D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, creationFlags, NULL, NULL, D3D11_SDK_VERSION, &swapDesc, &m_swapChain, &m_device, NULL, &m_deviceContext);

    // 4. ���� Ÿ�� �� ���� (�� ���۸� �̿��ϴ� ���� Ÿ�� ��)
    ID3D11Texture2D* BackBufferTexture = nullptr;

    m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&BackBufferTexture);
    m_device->CreateRenderTargetView(BackBufferTexture, NULL, &m_renderTargetView);   // �ؽ�ó�� ���� ���� ����
    BackBufferTexture->Release();                                                              // �ܺ� ���� ī��Ʈ�� ���ҽ�Ų��

    // ���� Ÿ���� ���� ��� ���������ο� ���ε�
    // Flip Mode�� �ƴ� ������ ���� �ѹ��� �����ϸ� ��
    m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, NULL);

    // 5. ����Ʈ ����
    D3D11_VIEWPORT viewport = {};
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = (float)width;
    viewport.Height = (float)height;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    m_deviceContext->RSSetViewports(1, &viewport);

    // 6. ���� & ���ٽ� �� ����
    D3D11_TEXTURE2D_DESC descDepth = {};
    descDepth.Width = width;
    descDepth.Height = height;
    descDepth.MipLevels = 1;
    descDepth.ArraySize = 1;
    descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    descDepth.SampleDesc.Count = 1;
    descDepth.SampleDesc.Quality = 0;
    descDepth.Usage = D3D11_USAGE_DEFAULT;
    descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    descDepth.CPUAccessFlags = 0;
    descDepth.MiscFlags = 0;

    ID3D11Texture2D* textureDepthStencil = nullptr;
    m_device->CreateTexture2D(&descDepth, nullptr, &textureDepthStencil);

    D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
    descDSV.Format = descDepth.Format;
    descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    descDSV.Texture2D.MipSlice = 0;
    m_device->CreateDepthStencilView(textureDepthStencil, &descDSV, &m_depthStencilView);
    textureDepthStencil->Release();

    m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);

    // 7. ��� ���� ����
    D3D11_BUFFER_DESC bd = {};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(CB_Transform);
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = 0;
    m_device->CreateBuffer(&bd, nullptr, &m_transformCB);

    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(CB_DirectionalLight);
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = 0;
    m_device->CreateBuffer(&bd, nullptr, &m_directionalLightCB);

    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(CB_Material);
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = 0;
    m_device->CreateBuffer(&bd, nullptr, &m_materialCB);

    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(CB_MatrixPalette);
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = 0;
    m_device->CreateBuffer(&bd, nullptr, &m_matrixPaletteCB);

    // 8. Sample state ����
    D3D11_SAMPLER_DESC sampDesc = {};
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
    m_device->CreateSamplerState(&sampDesc, &m_samplerLinear);

    // 9. ���� ó���� ���� ���� ���� ����
    D3D11_BLEND_DESC blendDesc = {};
    blendDesc.AlphaToCoverageEnable = false;
    blendDesc.IndependentBlendEnable = false;
    blendDesc.RenderTarget[0].BlendEnable = true;
    blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
    blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    m_device->CreateBlendState(&blendDesc, &m_alphaBlendState);

    // 10. ImGUI �ʱ�ȭ
    InitImGUI();

    return true;
}

void D3DRenderManager::UnInitialize()
{
    SAFE_RELEASE(m_directionalLightCB);
    SAFE_RELEASE(m_transformCB);
    SAFE_RELEASE(m_materialCB);
    SAFE_RELEASE(m_matrixPaletteCB);
    SAFE_RELEASE(m_samplerLinear);
    SAFE_RELEASE(m_alphaBlendState);

    SAFE_RELEASE(m_renderTargetView);
    SAFE_RELEASE(m_depthStencilView);
    SAFE_RELEASE(m_deviceContext);
    SAFE_RELEASE(m_swapChain);
    SAFE_RELEASE(m_device);

    UnInitImGUI();
}

void D3DRenderManager::Update()
{
    // ī�޶� ������Ʈ
    // �޽� �ø�
}

void D3DRenderManager::Render()
{
    // ȭ�� ĥ�ϱ�
    m_deviceContext->ClearRenderTargetView(m_renderTargetView, m_clearColor);
    m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

    // Draw �迭 �Լ��� ȣ���ϱ� ���� ������ ���������ο� �ʼ� �������� ������ �ؾ��Ѵ�.
    m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    m_deviceContext->IASetInputLayout(m_inputLayout);

    m_deviceContext->VSSetShader(m_vertexShader, nullptr, 0);
    m_deviceContext->PSSetShader(m_pixelShader, nullptr, 0);

    m_deviceContext->PSSetConstantBuffers(1, 1, &m_directionalLightCB);

    m_deviceContext->PSSetSamplers(0, 1, &m_samplerLinear);

    // ViewMatrix, ProjectionMatrix ����
    m_transform.ViewMatrix = DirectX::XMMatrixTranspose(m_viewMatrix);
    m_transform.ProjectionMatrix = DirectX::XMMatrixTranspose(m_projectionMatrix);

    m_deviceContext->UpdateSubresource(m_transformCB, 0, nullptr, &m_transform, 0, 0);
    m_deviceContext->VSSetConstantBuffers(0, 1, &m_transformCB);
    m_deviceContext->PSSetConstantBuffers(0, 1, &m_transformCB);

    // Light ����
    m_light.Direction.Normalize(); 
    m_deviceContext->UpdateSubresource(m_directionalLightCB, 0, nullptr, &m_light, 0, 0);

    RenderImGUI();

    m_swapChain->Present(0, 0);
}

bool D3DRenderManager::InitImGUI()
{
    // ImGui �ʱ�ȭ
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    // ImGui ��Ÿ�� ����
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // �÷���, ������ ����
    ImGui_ImplWin32_Init(m_hWnd);
    ImGui_ImplDX11_Init(m_device, m_deviceContext);

    return true;
}

void D3DRenderManager::UnInitImGUI()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

void D3DRenderManager::RenderImGUI()
{
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    {
        ImGui::Begin("D3D Renderer������ ImGui �׽�Ʈ");
        ImGui::End();
    }
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}
