#include "RowCounter.h"
#include <format>

namespace trview
{
    namespace
    {
        constexpr std::string plural(std::size_t count)
        {
            return (count == 0 || count > 1) ? "s" : "";
        }
    }

    RowCounter::RowCounter(const std::string& entry_name, std::size_t maximum_size)
        : _entry_name(entry_name), _maximum_size(maximum_size), _count(0)
    {
    }

    float RowCounter::height() const
    {
        return ImGui::CalcTextSize(_entry_name.c_str()).y * 1.5f;
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
        const bool is_filtered = _count != _maximum_size;
        const std::string text =
            is_filtered ?
            std::format("Results are filtered. {} {}{} out of {}.", _count, _entry_name, plural(_count), _maximum_size) :
            std::format("{} {}{}", _maximum_size, _entry_name, plural(_count));
        const ImVec2 size = ImGui::CalcTextSize(text.c_str());
        ImGui::SetCursorPosX(
            is_filtered ? 
                ImGui::GetWindowWidth() * 0.5f - size.x * 0.5f :
                ImGui::GetWindowWidth() - size.x - 5);
        ImGui::Text(text.c_str());
    }
}
