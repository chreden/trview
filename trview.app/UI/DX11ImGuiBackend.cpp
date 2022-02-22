#include "DX11ImGuiBackend.h"

#include <external/imgui/backends/imgui_impl_win32.h>
#include <external/imgui/backends/imgui_impl_dx11.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace trview
{
    DX11ImGuiBackend::DX11ImGuiBackend(const Window& window, const std::shared_ptr<graphics::IDevice>& device)
        : _window(window), _device(device)
    {
    }

    void DX11ImGuiBackend::initialise()
    {
        ImGui_ImplWin32_Init(_window);
        ImGui_ImplDX11_Init(_device->device().Get(), _device->context().Get());
    }

    void DX11ImGuiBackend::new_frame()
    {
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
    }

    void DX11ImGuiBackend::render()
    {
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    }

    void DX11ImGuiBackend::shutdown()
    {
        ImGui_ImplDX11_Shutdown();
        ImGui_ImplWin32_Shutdown();
    }

    bool DX11ImGuiBackend::window_procedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        return ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam);
    }
}