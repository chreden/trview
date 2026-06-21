#include "NullImGuiBackend.h"
#include <external/imgui/imgui.h>
#include <external/imgui/imgui_internal.h>

#include <external/imgui/backends/imgui_impl_null.h>

namespace trview
{
    namespace tests
    {
        void NullImGuiBackend::initialise()
        {
            auto context = ImGui::GetCurrentContext();
            context->IO.DisplaySize = ImVec2(1920, 1080);
            ImGui_ImplNull_Init();
            _active = true;
        }

        bool NullImGuiBackend::is_setup() const
        {
            return _active;
        }

        void NullImGuiBackend::new_frame()
        {
            ImGui_ImplNull_NewFrame();
        }

        void NullImGuiBackend::rebuild_fonts()
        {
        }

        void NullImGuiBackend::render()
        {
            ImGui_ImplNullRender_RenderDrawData(ImGui::GetDrawData());
        }

        void NullImGuiBackend::reset_layout()
        {
        }

        void NullImGuiBackend::shutdown()
        {
            ImGui_ImplNull_Shutdown();
            _active = false;
        }

        bool NullImGuiBackend::window_procedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
        {
            return false;
        }
    }
}