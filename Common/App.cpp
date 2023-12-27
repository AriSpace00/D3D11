#include "pch.h"
#include "App.h"

App* App::m_Instance = nullptr;
HWND App::m_hWnd;

LRESULT DefaultWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    return App::m_Instance->WndProc(hWnd, message, wParam, lParam);
}

App::App(HINSTANCE hInstance)
    : m_hInstance(hInstance)
    , m_szTitle(L"New Window")
    , m_szWindowClass(L"App")
{
    App::m_Instance = this;

    m_wcex.cbSize = sizeof(WNDCLASSEX);
    m_wcex.style = CS_HREDRAW | CS_VREDRAW;
    m_wcex.lpfnWndProc = DefaultWndProc;
    m_wcex.cbClsExtra = 0;
    m_wcex.cbWndExtra = 0;
    m_wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    m_wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    m_wcex.lpszClassName = m_szWindowClass;
}

App::~App()
{
}

bool App::Initialize(UINT width, UINT height)
{
    m_clientWidth = width;
    m_clientHeight = height;

    RegisterClassExW(&m_wcex);

    RECT rcClient = { 0, 0, (LONG)width, (LONG)height };
    AdjustWindowRect(&rcClient, WS_OVERLAPPEDWINDOW, FALSE);

    int m_ScreenWidth = GetSystemMetrics(SM_CXSCREEN);
    int m_ScreenHeight = GetSystemMetrics(SM_CYSCREEN);

    int windowPosX = (m_ScreenWidth / 2) - ((rcClient.right - rcClient.left) / 2);
    int windowPosY = (m_ScreenHeight / 2) - ((rcClient.bottom - rcClient.top) / 2);

    m_hWnd = CreateWindowW(m_szWindowClass, m_szTitle, WS_OVERLAPPEDWINDOW,
        windowPosX, windowPosY,                                 
        rcClient.right - rcClient.left,                         
        rcClient.bottom - rcClient.top,                         
        nullptr, nullptr, m_hInstance, nullptr);

    if (!m_hWnd)
        return false;

    ShowWindow(m_hWnd, SW_SHOW);
    UpdateWindow(m_hWnd);

    return true;
}

bool App::Run()
{
    while (TRUE)
    {
        if (PeekMessage(&m_msg, NULL, 0, 0, PM_REMOVE))
        {
            if (m_msg.message == WM_QUIT)
                break;

            TranslateMessage(&m_msg);   
            DispatchMessage(&m_msg);
        }
        else
        {
            Update();
            Render();
        }
    }
    return 0;
}

void App::Update()
{
}

void App::Render()
{
}

LRESULT App::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
