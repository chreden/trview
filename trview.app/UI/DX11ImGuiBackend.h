#pragma once

#include <trview.common/IFiles.h>
#include <trview.graphics/IDevice.h>
#include "IImGuiBackend.h"

namespace trview
{
    class DX11ImGuiBackend : public IImGuiBackend
    {
    public:
        DX11ImGuiBackend(const Window& window, const std::shared_ptr<graphics::IDevice>& device, const std::shared_ptr<IFiles>& files);
        virtual ~DX11ImGuiBackend() = default;
        void initialise() override;
        bool is_setup() const override;
        void new_frame() override;
        void rebuild_fonts() override;
        void render() override;
        void reset_layout() override;
        void shutdown() override;
        bool window_procedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) override;
    private:
        std::shared_ptr<graphics::IDevice> _device;
        std::shared_ptr<IFiles> _files;
        Window _window;
        bool _active{ false };
    };
}
