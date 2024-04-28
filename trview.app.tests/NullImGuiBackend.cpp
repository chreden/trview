#include "NullImGuiBackend.h"
#include <external/imgui/imgui.h>
#include <external/imgui/imgui_internal.h>

namespace trview
{
    namespace tests
    {
        void NullImGuiBackend::initialise()
        {
            auto context = ImGui::GetCurrentContext();
            context->IO.DisplaySize = ImVec2(1920, 1080);
            _active = true;
        }

        bool NullImGuiBackend::is_setup() const
        {
            return _active;
        }

        void NullImGuiBackend::new_frame()
        {
            auto context = ImGui::GetCurrentContext();

            unsigned char* data = 0;
            int32_t width, height, bpp;
            context->IO.Fonts->GetTexDataAsRGBA32(&data, &width, &height, &bpp);
        }

        void NullImGuiBackend::rebuild_fonts()
        {
        }

        void NullImGuiBackend::render()
        {
        }

        void NullImGuiBackend::reset_layout()
        {
        }

        void NullImGuiBackend::shutdown()
        {
            _active = false;
        }

        bool NullImGuiBackend::window_procedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
        {
            return false;
        }
    }
}