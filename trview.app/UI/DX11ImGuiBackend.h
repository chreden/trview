#pragma once

#include <trview.graphics/IDevice.h>
#include "IImGuiBackend.h"

namespace trview
{
    class DX11ImGuiBackend : public IImGuiBackend
    {
    public:
        DX11ImGuiBackend(const Window& window, const std::shared_ptr<graphics::IDevice>& device);
        virtual ~DX11ImGuiBackend() = default;
        virtual void initialise() override;
        virtual void new_frame() override;
        virtual void render() override;
        virtual void shutdown() override;
        virtual bool window_procedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) override;
    private:
        std::shared_ptr<graphics::IDevice> _device;
        Window _window;
    };
}
