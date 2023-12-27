#include "DemoApp.h"
#include "ImGUI.h"

#include "Common/Helper.h"
#include "Common/Model.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

DemoApp::DemoApp(HINSTANCE hInstance)
    : App(hInstance)
    , m_ImGUI(new ImGUI())
    , m_CameraNear(1.0f)
    , m_CameraFar(9999.9f)
    , m_CameraFovYRadius(90.0f)
{
}

DemoApp::~DemoApp()
{
    m_ImGUI->UnInitialize();
    UnInitScene();
    UnInitD3D();
}

bool DemoApp::Initialize(UINT width, UINT height)
{
    App::Initialize(width, height);

    if (!InitD3D())
        return false;

    m_ImGUI->Initialize(App::m_hWnd, m_Device, m_DeviceContext);

    if (!InitScene())
        return false;

    return true;
}

void DemoApp::Update()
{
    App::Update();

    QueryPerformanceCounter(&m_CurrentTime);
    m_DeltaTime = static_cast<double>(m_CurrentTime.QuadPart - m_PrevTime.QuadPart) / m_Frequency.QuadPart;
    m_PrevTime = m_CurrentTime;

    m_Model->Update(m_DeltaTime);

    Matrix mSpin = XMMatrixRotationRollPitchYaw(XMConvertToRadians(m_Pitch), XMConvertToRadians(m_Yaw), XMConvertToRadians(m_Roll));
    Matrix mScale = Matrix::CreateScale(m_MeshScale, m_MeshScale, m_MeshScale);
    m_Model->SetTransform(DirectX::XMMatrixIdentity(), mSpin, mScale);

    m_Light.EyePosition = m_Eye;
}

void DemoApp::Render()
{
    App::Render();

    // ȭ�� ĥ�ϱ�
    m_DeviceContext->ClearRenderTargetView(m_RenderTargetView, m_ClearColor);
    m_DeviceContext->ClearDepthStencilView(m_DepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

    // Draw �迭 �Լ��� ȣ���ϱ� ���� ������ ���������ο� �ʼ� �������� ������ �ؾ��Ѵ�.
    m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    m_DeviceContext->IASetInputLayout(m_InputLayout);

    m_DeviceContext->VSSetShader(m_VertexShader, nullptr, 0);
    m_DeviceContext->PSSetShader(m_PixelShader, nullptr, 0);

    m_DeviceContext->PSSetConstantBuffers(1, 1, &m_CBDirectionalLight);

    m_DeviceContext->PSSetSamplers(0, 1, &m_SamplerLinear);

    // ViewMatrix, ProjectionMatrix ����
    m_Model->m_transform.ViewMatrix = XMMatrixTranspose(m_ViewMatrix);
    m_Model->m_transform.ProjectionMatrix = XMMatrixTranspose(m_ProjectionMatrix);

    m_DeviceContext->UpdateSubresource(m_Model->m_transformCB, 0, nullptr, &m_Model->m_transform, 0, 0);
    m_DeviceContext->VSSetConstantBuffers(0, 1, &m_Model->m_transformCB);
    m_DeviceContext->PSSetConstantBuffers(0, 1, &m_Model->m_transformCB);

    // Light ����
    m_Light.Direction.Normalize();
    m_DeviceContext->UpdateSubresource(m_CBDirectionalLight, 0, nullptr, &m_Light, 0, 0);

    // Model Render
    m_Model->Render(m_DeviceContext);

    RunImGUI();

    m_SwapChain->Present(0, 0);
}

bool DemoApp::InitD3D()
{
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
    swapDesc.BufferDesc.Width = m_clientWidth;
    swapDesc.BufferDesc.Height = m_clientHeight;

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
    D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, creationFlags, NULL, NULL, D3D11_SDK_VERSION, &swapDesc, &m_SwapChain, &m_Device, NULL, &m_DeviceContext);

    // 4. ���� Ÿ�� �� ���� (�� ���۸� �̿��ϴ� ���� Ÿ�� ��)
    ID3D11Texture2D* BackBufferTexture = nullptr;

    m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&BackBufferTexture);
    m_Device->CreateRenderTargetView(BackBufferTexture, NULL, &m_RenderTargetView);   // �ؽ�ó�� ���� ���� ����
    BackBufferTexture->Release();                                                              // �ܺ� ���� ī��Ʈ�� ���ҽ�Ų��

    // ���� Ÿ���� ���� ��� ���������ο� ���ε�
    // Flip Mode�� �ƴ� ������ ���� �ѹ��� �����ϸ� ��
    m_DeviceContext->OMSetRenderTargets(1, &m_RenderTargetView, NULL);

    // 5. ����Ʈ ����
    D3D11_VIEWPORT viewport = {};
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = (float)m_clientWidth;
    viewport.Height = (float)m_clientHeight;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    m_DeviceContext->RSSetViewports(1, &viewport);

    // 6. ���� & ���ٽ� �� ����
    D3D11_TEXTURE2D_DESC descDepth = {};
    descDepth.Width = m_clientWidth;
    descDepth.Height = m_clientHeight;
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
    m_Device->CreateTexture2D(&descDepth, nullptr, &textureDepthStencil);

    D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
    descDSV.Format = descDepth.Format;
    descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    descDSV.Texture2D.MipSlice = 0;
    m_Device->CreateDepthStencilView(textureDepthStencil, &descDSV, &m_DepthStencilView);
    textureDepthStencil->Release();

    m_DeviceContext->OMSetRenderTargets(1, &m_RenderTargetView, m_DepthStencilView);

    return true;
}

void DemoApp::UnInitD3D()
{
    m_RenderTargetView->Release();
    m_DepthStencilView->Release();
    m_Device->Release();
    m_DeviceContext->Release();
    m_SwapChain->Release();
}

bool DemoApp::InitScene()
{
    HRESULT hr = 0;
    ID3D10Blob* errorMessage = nullptr;

    std::string filePath;
    std::string zelda = "../Resource/FBXLoad_Test/fbx/zeldaPosed001.fbx";
    std::string boneDummy = "../Resource/FBXLoad_Test/fbx/BoneDummyWithMaterial.fbx";
    std::string skinningFBX = "../Resource/FBXLoad_Test/fbx/SkinningTest.fbx";
    std::string cerberus = "../Resource/FBXLoad_Test/fbx/cerberus2.fbx";

    filePath = cerberus;

    // Model Ŭ������ FBX Load
    m_Model = new Model();
    m_Model->ReadFile(m_Device, filePath);

    ID3DBlob* vertexShaderBuffer = nullptr;
    CompileShaderFromFile(L"VertexShader.hlsl", "main", "vs_5_0", &vertexShaderBuffer);

    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        {"NORMAL",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TANGENT",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"BlendIndices",    0, DXGI_FORMAT_R32G32B32A32_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"BlendWeights",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };

    m_Device->CreateInputLayout(layout, ARRAYSIZE(layout), vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_InputLayout);

    // 3. Render() ���� ���������ο� ���ε��� ���ؽ� ���̴� ����
    m_Device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_VertexShader);
    vertexShaderBuffer->Release();

    // 4. Render() ���� ���������ο� ���ε��� �ε��� ���� ����

    // 5. Render() ���� ���������ο� ���ε��� �ȼ� ���̴� ����
    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;

    ID3DBlob* pixelShaderBuffer = nullptr;
    CompileShaderFromFile(L"PBRPixelShader.hlsl", "main", "ps_5_0", &pixelShaderBuffer);
    
    m_Device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_PixelShader);
    pixelShaderBuffer->Release();

    // 6. Render() ���� ���������ο� ���ε��� ��� ���� ����
    D3D11_BUFFER_DESC bd = {};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(CB_Transform);
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = 0;
    m_Device->CreateBuffer(&bd, nullptr, &m_Model->m_transformCB);

    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(CB_DirectionalLight);
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = 0;
    m_Device->CreateBuffer(&bd, nullptr, &m_CBDirectionalLight);

    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(CB_Material);
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = 0;
    m_Device->CreateBuffer(&bd, nullptr, &m_Model->m_materialCB);

    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(CB_MatrixPalette);
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = 0;
    m_Device->CreateBuffer(&bd, nullptr, &m_Model->m_matrixPaletteCB);

    // Sample state ����
    D3D11_SAMPLER_DESC sampDesc = {};
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
    m_Device->CreateSamplerState(&sampDesc, &m_SamplerLinear);

    // 7. ���̴��� ������ ������ ����

    // �� ��Ʈ���� �ʱ�ȭ
    m_Eye = XMVectorSet(0.0f, 300.0f, -500.0f, 0.0f);
    m_At = XMVectorSet(0.0f, 0.0f, 0.1f, 0.0f);
    m_Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

    m_ViewMatrix = XMMatrixLookToLH(m_Eye, m_At, m_Up);

    // �������� ��Ʈ���� �ʱ�ȭ
    m_ProjectionMatrix = XMMatrixPerspectiveFovLH(XM_PIDIV4, m_clientWidth / (FLOAT)m_clientHeight, 1.0f, 20000.0f);

    // 7. ���� ó���� ���� ���� ���� ����
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

    m_Device->CreateBlendState(&blendDesc, &m_Model->m_alphaBlendState);

    // Get the frequency of the performance counter
    QueryPerformanceFrequency(&m_Frequency);

    // Get the initial time
    QueryPerformanceCounter(&m_PrevTime);

    return true;
}

void DemoApp::UnInitScene()
{
    m_Meshes.clear();
    m_Materials.clear();

    m_CBDirectionalLight->Release();
    m_VertexShader->Release();
    m_PixelShader->Release();
    m_InputLayout->Release();
}

void DemoApp::RunImGUI()
{
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;           // Ű���� �Է°� �ޱ�
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;            // �����е� �Է°� �ޱ�

    m_ImGUI->BeginRender();

    // 1. ť�� ���� ������
    ImGui::SetNextWindowSize(ImVec2(350, 150));
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    {
        ImGui::Begin("Mesh Properties");

        ImGui::Text("Rotation");
        ImGui::Text("X");
        ImGui::SameLine();
        ImGui::SliderFloat("##cx", &m_Roll, -360.0f, 360.0f);
        ImGui::Text("Y");
        ImGui::SameLine();
        ImGui::SliderFloat("##cy", &m_Pitch, -360.0f, 360.0f);
        ImGui::Text("Z");
        ImGui::SameLine();
        ImGui::SliderFloat("##cz", &m_Yaw, -360.0f, 360.0f);
        ImGui::Text("Scale");
        ImGui::SameLine();
        ImGui::SliderFloat("##cs", &m_MeshScale, 0.0f, 100.0f);

        ImGui::End();
    }

    // 2. ī�޶� ���� ������
    ImGui::SetNextWindowSize(ImVec2(350, 250));
    ImGui::SetNextWindowPos(ImVec2(0, 150));
    {
        ImGui::Begin("Camera Properties");

        ImGui::Text("World Transform");
        float x = XMVectorGetX(m_Eye);
        float y = XMVectorGetY(m_Eye);
        float z = XMVectorGetZ(m_Eye);
        ImGui::Text("X");
        ImGui::SameLine();
        ImGui::SliderFloat("##cwx", &x, -1000.0f, 10000.0f);
        ImGui::Text("Y");
        ImGui::SameLine();
        ImGui::SliderFloat("##cwy", &y, -1000.0f, 10000.0f);
        ImGui::Text("Z");
        ImGui::SameLine();
        ImGui::SliderFloat("##cwz", &z, -10000.0f, 0.0f);
        m_Eye = DirectX::XMVectorSet(x, y, z, 0.0f);
        m_ViewMatrix = XMMatrixLookToLH(m_Eye, m_At, m_Up);


        ImGui::Text("FOV Degree");
        ImGui::Text("Y");
        ImGui::SameLine();
        ImGui::SliderFloat("##cfx", &m_CameraFovYRadius, 0.01f, 180.0f);
        float fovRadius = m_CameraFovYRadius * (DirectX::XM_PI / 180.0f);

        ImGui::Text("Near / Far");
        ImGui::Text("Near");
        ImGui::SameLine();
        ImGui::SliderFloat("##cn", &m_CameraNear, 1.0f, 9999.9f);
        ImGui::Text("Far ");
        ImGui::SameLine();
        ImGui::SliderFloat("##cf", &m_CameraFar, 0.01f, 9999.9f);
        if (m_CameraNear < m_CameraFar)
        {
            m_ProjectionMatrix = XMMatrixPerspectiveFovLH(fovRadius, m_clientWidth / (FLOAT)m_clientHeight, m_CameraNear, m_CameraFar);
        }

        ImGui::End();
    }

    ImGui::SetNextWindowSize(ImVec2(350, 450));
    ImGui::SetNextWindowPos(ImVec2(0, 400));
    {
        ImGui::Begin("Light Properties");

        ImGui::Text("[Directional Light]");
        ImGui::Text("Light Direction");
        ImGui::SliderFloat3("##ldir", (float*)&m_Light.Direction, -1.0f, 1.0f);
        ImGui::Text("Light Ambient");
        ImGui::ColorEdit4("##la", (float*)&m_Light.Ambient);
        ImGui::Text("Light Diffuse");
        ImGui::ColorEdit4("##ldiff", (float*)&m_Light.Diffuse);
        ImGui::Text("Light Specular");
        ImGui::ColorEdit4("##ls", (float*)&m_Light.Specular);

        ImGui::Text("[Material]");
        ImGui::Text("Material Ambient");
        ImGui::ColorEdit4("##ma", (float*)&m_Model->m_material.Ambient);
        ImGui::Text("Material Diffuse");
        ImGui::ColorEdit4("##md", (float*)&m_Model->m_material.Diffuse);
        ImGui::Text("Material Specular");
        ImGui::ColorEdit4("##ms", (float*)&m_Model->m_material.Specular);
        ImGui::Text("Material Specular Power");
        ImGui::SliderFloat("##sp", &m_Model->m_material.SpecularPower, 2.0f, 4096.0f);

        ImGui::End();
    }

    m_ImGUI->EndRender();
}

LRESULT DemoApp::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (m_ImGUI->HandleMessage(hWnd, message, wParam, lParam))
        return true;

    return __super::WndProc(hWnd, message, wParam, lParam);
}
