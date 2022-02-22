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
        }

        void NullImGuiBackend::new_frame()
        {
            auto context = ImGui::GetCurrentContext();

            unsigned char* data = 0;
            int32_t width, height, bpp;
            context->IO.Fonts->GetTexDataAsRGBA32(&data, &width, &height, &bpp);
        }

        void NullImGuiBackend::render()
        {
        }

        void NullImGuiBackend::shutdown()
        {
        }

        bool NullImGuiBackend::window_procedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
        {
            return false;
        }
    }
}