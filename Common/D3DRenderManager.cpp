#include "pch.h"
#include "D3DRenderManager.h"

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

    return true;
}

void D3DRenderManager::UnInitialize()
{
}

void D3DRenderManager::Update()
{
}

void D3DRenderManager::Render()
{
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