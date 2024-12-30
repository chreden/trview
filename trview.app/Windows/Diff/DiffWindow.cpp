#include "DiffWindow.h"

namespace trview
{
    IDiffWindow::~IDiffWindow()
    {
    }

    DiffWindow::DiffWindow()
    {
    }

    void DiffWindow::render()
    {
        if (!render_diff_window())
        {
            on_window_closed();
            return;
        }
    }

    bool DiffWindow::render_diff_window()
    {
        bool stay_open = true;
        ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(520, 400));
        if (ImGui::Begin(_id.c_str(), &stay_open))
        {
        }
        ImGui::End();
        ImGui::PopStyleVar();
        return stay_open;
    }

    void DiffWindow::set_number(int32_t number)
    {
        _id = std::format("Diff {}", number);
    }
}

