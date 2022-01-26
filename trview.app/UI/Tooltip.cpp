#include "Tooltip.h"

namespace trview
{
    void Tooltip::set_text(const std::wstring& text)
    {
        _text = to_utf8(text);
    }

    void Tooltip::set_text_colour(const Colour& colour)
    {
        _colour = colour;
    }

    void Tooltip::set_visible(bool value)
    {
        _visible = value;
    }

    bool Tooltip::visible() const
    {
        return _visible;
    }

    void Tooltip::render()
    {
        if (_visible)
        {
            ImGui::BeginTooltip();
            ImGui::TextColored(ImVec4(_colour.r, _colour.g, _colour.b, _colour.a), _text.c_str());
            ImGui::End();
        }
    }
}
