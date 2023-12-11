#pragma once
#include "framework.h"

#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>

struct ID3D11Device;
struct ID3D11DeviceContext;

class ImGUI
{
public:
    ImGUI();
    ~ImGUI();

public:
    bool Initialize(HWND hWnd, ID3D11Device* device, ID3D11DeviceContext* deviceContext);
    void UnInitialize();

    void BeginRender();
    void EndRender();

    bool HandleMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};

