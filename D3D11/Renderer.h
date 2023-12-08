#pragma once

#include "Common/App.h"

class Renderer :
    public App
{
public:
    Renderer(HINSTANCE hInstance);
    ~Renderer();

public:
    virtual bool Initialize(UINT width, UINT height) override;
    virtual void Update() override;
    virtual void Render() override;

    bool InitD3D();
    void UnInitD3D();

    bool InitScene();
    void unInitScene();

    bool InitImGUI();
    void UnInitImGUI();

    virtual LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) override;
};

