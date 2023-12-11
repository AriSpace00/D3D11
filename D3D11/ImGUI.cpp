#include "ImGUI.h"

ImGUI::ImGUI()
{
}

ImGUI::~ImGUI()
{
}

bool ImGUI::Initialize(HWND hWnd, ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
    // ImGui �ʱ�ȭ
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    // ImGui ��Ÿ�� ����
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // �÷���, ������ ����
    ImGui_ImplWin32_Init(hWnd);
    ImGui_ImplDX11_Init(device, deviceContext);

    return true;
}

void ImGUI::UnInitialize()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

void ImGUI::BeginRender()
{
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

void ImGUI::EndRender()
{
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

bool ImGUI::HandleMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
        return true;

    return false;
}