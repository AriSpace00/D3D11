#pragma once
#define MAX_LOADSTRING 100

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

public:
    virtual bool Initialize(UINT width, UINT height);
    virtual bool Run();
    virtual void Update();
    virtual void Render();

    virtual LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};

