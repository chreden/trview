#include "StaticsWindow.h"

namespace trview
{
    IStaticsWindow::~IStaticsWindow()
    {
    }

    StaticsWindow::StaticsWindow()
    {
    }

    void StaticsWindow::render()
    {
        if (!render_statics_window())
        {
            on_window_closed();
            return;
        }
    }

    bool StaticsWindow::render_statics_window()
    {
        bool stay_open = true;
        if (ImGui::Begin(_id.c_str(), &stay_open))
        {
            calculate_column_widths();
            ImGui::Text("Statics window?");
        }
        ImGui::End();
        return stay_open;
    }

    void StaticsWindow::set_number(int32_t number)
    {
        _id = std::format("Statics {}", number);
    }

    void StaticsWindow::update(float)
    {
    }

    void StaticsWindow::calculate_column_widths()
    {
        _column_sizer.reset();
        /*
        _column_sizer.measure("#__", 0);
        _column_sizer.measure("Room__", 1);
        _column_sizer.measure("ID__", 2);
        _column_sizer.measure("Type__", 3);
        _column_sizer.measure("Hide____", 4);

        for (const auto& item : _all_items)
        {
            if (auto item_ptr = item.lock())
            {
                const bool item_is_virtual = is_virtual(*item_ptr);
                _column_sizer.measure(std::format("{0}{1}##{0}", item_ptr->number(), item_is_virtual ? "*" : ""), 0);
                _column_sizer.measure(std::to_string(item_room(item_ptr)), 1);
                _column_sizer.measure(std::to_string(item_ptr->type_id()), 2);
                _column_sizer.measure(item_ptr->type(), 3);
            }
        }
        */
    }
}
