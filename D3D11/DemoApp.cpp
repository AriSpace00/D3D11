#include "DemoApp.h"

#include "Common/Helper.h"
#include "Common/Model.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

DemoApp::DemoApp(HINSTANCE hInstance)
    : App(hInstance)
    , m_CameraNear(1.0f)
    , m_CameraFar(9999.9f)
    , m_CameraFovYRadius(90.0f)
{
}

DemoApp::~DemoApp()
{
}

bool DemoApp::Initialize(UINT width, UINT height)
{
    App::Initialize(width, height);

    return true;
}

void DemoApp::Update()
{
    App::Update();

    QueryPerformanceCounter(&m_CurrentTime);
    m_DeltaTime = static_cast<double>(m_CurrentTime.QuadPart - m_PrevTime.QuadPart) / m_Frequency.QuadPart;
    m_PrevTime = m_CurrentTime;

    //m_Model->Update(m_DeltaTime);

    Matrix mSpin = XMMatrixRotationRollPitchYaw(XMConvertToRadians(m_Pitch), XMConvertToRadians(m_Yaw), XMConvertToRadians(m_Roll));
    Matrix mScale = Matrix::CreateScale(m_MeshScale, m_MeshScale, m_MeshScale);
    //m_Model->SetTransform(DirectX::XMMatrixIdentity(), mSpin, mScale);

}

void DemoApp::Render()
{
    App::Render();
}

LRESULT DemoApp::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    return __super::WndProc(hWnd, message, wParam, lParam);
}
