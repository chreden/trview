#include "DX11ImGuiBackend.h"

#include <external/imgui/backends/imgui_impl_win32.h>
#include <external/imgui/backends/imgui_impl_dx11.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace trview
{
    IImGuiBackend::~IImGuiBackend()
    {
    }

    DX11ImGuiBackend::DX11ImGuiBackend(const Window& window, const std::shared_ptr<graphics::IDevice>& device, const std::shared_ptr<IFiles>& files)
        : _window(window), _device(device), _files(files)
    {
    }

    void DX11ImGuiBackend::initialise()
    {
        ImGui_ImplWin32_Init(_window);
        ImGui_ImplDX11_Init(_device->device().Get(), _device->context().Get());
        _active = true;
    }

    bool DX11ImGuiBackend::is_setup() const
    {
        return _active;
    }

    void DX11ImGuiBackend::new_frame()
    {
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
    }

    void DX11ImGuiBackend::rebuild_fonts()
    {
        auto& io = ImGui::GetIO();
        io.Fonts->Build();
        ImGui_ImplDX11_InvalidateDeviceObjects();
    }

    void DX11ImGuiBackend::render()
    {
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    }

    void DX11ImGuiBackend::reset_layout()
    {
        auto& io = ImGui::GetIO();
        _files->delete_file(io.IniFilename);
        io.IniFilename = nullptr;
        shutdown();
    }

    void DX11ImGuiBackend::shutdown()
    {
        ImGui_ImplDX11_Shutdown();
        ImGui_ImplWin32_Shutdown();
        _active = false;
    }

    bool DX11ImGuiBackend::window_procedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        if (_active)
        {
            return ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam);
        }
        return false;
    }
}