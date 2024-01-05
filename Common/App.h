#pragma once
#include "D3DRenderManager.h"
#include "ResourceManager.h"
#include "TimeManager.h"

#define MAX_LOADSTRING 100

class World;

class App
{
public:
    App(HINSTANCE hInstance);
    virtual ~App();

    static HWND m_hWnd;
    static App* m_Instance;

public:
    MSG m_msg;
    WCHAR m_szTitle[MAX_LOADSTRING];        
    WCHAR m_szWindowClass[MAX_LOADSTRING];  
    HACCEL m_hAccelTable;
    HINSTANCE m_hInstance;                  
    WNDCLASSEXW m_wcex;

    UINT m_clientWidth;
    UINT m_clientHeight;

    World* m_currentWorld = nullptr;

    D3DRenderManager m_renderer;
    ResourceManager m_resource;
    TimeManager m_timer;

public:
    virtual bool Initialize(UINT width, UINT height);
    virtual bool Run();
    virtual void Update();
    virtual void Render();

    void ChangeWorld(World* world);

    virtual LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};

