#include "RowCounter.h"
#include <format>

namespace trview
{
    RowCounter::RowCounter(const std::string& entry_name, std::size_t maximum_size)
        : _entry_name(entry_name), _maximum_size(maximum_size), _count(0)
    {
    }

    float RowCounter::height() const
    {
        return ImGui::CalcTextSize(_entry_name.c_str()).y;
    }

    void RowCounter::count()
    {
        ++_count;
    }

    void RowCounter::set_count(std::size_t value)
    {
        _count = value;
    }

    void RowCounter::render()
    {
        const std::string text =
            _count == _maximum_size ?
            std::format("{} {}", _maximum_size, _entry_name) :
            std::format("{} of {} {}", _count, _maximum_size, _entry_name);
        const ImVec2 size = ImGui::CalcTextSize(text.c_str());
        ImGui::SetCursorPosX(ImGui::GetWindowWidth() - size.x - 5);
        ImGui::Text(text.c_str());
    }
}
