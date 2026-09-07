module;

#include <external/imgui/imgui.h>

export module trview.app:imgui;

import std;
import trview.common;

namespace trview
{
    export template < typename T >
    void imgui_sort(std::vector<T>& container, std::vector<std::function<bool(const T&, const T&)>> callbacks, bool force_sort = false);

    export template < typename T >
    void imgui_sort_weak(std::vector<std::weak_ptr<T>>& container, std::vector<std::function<bool(const T&, const T&)>> callbacks, bool force_sort = false);

    export struct ImGuiHeader
    {
        std::string name;
        float width;
        std::function<void(bool)> set_checked = nullptr;
        bool checked;
    };

    export void imgui_header_row(std::vector<ImGuiHeader> headers);

    export struct ImGuiScroller
    {
    public:
        ImGuiScroller();
        void scroll_to_item();
        void fix_scroll();
    private:
        ImVec2 _cursor_pos;
    };

    export struct ImGuiAnchor
    {
        enum class Anchor
        {
            TopLeft,
            TopRight,
            BottomLeft,
            BottomRight
        };

        std::optional<ImVec2> last_position;
        std::optional<ImVec2> last_padding;
        std::optional<ImVec2> last_size;
        std::optional<ImVec2> last_client_size;
        std::optional<ImVec2> in_window_offset;
        bool docked{ false };
        bool reposition{ false };
        Anchor anchor{ Anchor::TopRight };

        void check_resize(ImVec2 intended_client_size);
        void record_position(ImVec2 intended_client_size);
        void record_size();
    };

    export void read_only_colour_button(const std::string& name, const ImVec4& colour, IClipboard& clipboard);
}

#include "trview_imgui.hpp"
