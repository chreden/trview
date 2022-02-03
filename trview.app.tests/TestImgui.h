#pragma once

#define IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_ENABLE_TEST_ENGINE
#include <external/imgui/imgui.h>
#include <external/imgui/imgui_internal.h>
#include <external/imgui/backends/imgui_impl_win32.h>
#include <external/imgui/backends/imgui_impl_dx11.h>

#include <unordered_map>
#include <trview.common/Window.h>
#include <trview.graphics/Device.h>

namespace trview
{
    namespace tests
    {
        class TestImgui final
        {
        public:
            using RenderCallback = std::function<void()>;

            explicit TestImgui(const RenderCallback& callback);
            ~TestImgui();

            /// <summary>
            /// Click the specified element.
            /// </summary>
            /// <param name="window_name">The window that contains the element.</param>
            /// <param name="path_to_element">The path to the element including containing elements.</param>
            void click_element(const std::string& window_name, const std::vector<std::string>& path_to_element);

            void enter_text(const std::string& window_name, const std::vector<std::string>& path_to_element, const std::string& text);

            /// <summary>
            /// Add a control recangle.
            /// </summary>
            /// <param name="id">The ID of the control.</param>
            /// <param name="rect">The latest bounding rectangle.</param>
            void add_rect(ImGuiID id, const ImRect& rect);

            void add_status_flag(ImGuiID id, ImGuiItemStatusFlags flags);
            void add_item_flag(ImGuiID id, ImGuiItemFlags flags);
            void add_item_text(ImGuiID id, const std::string& text);
            void add_style_colours(ImGuiID id, const std::array<ImVec4, ImGuiCol_COUNT>& colours);

            bool element_present(const std::string& window_name, const std::vector<std::string>& path_to_element) const;

            ImGuiItemStatusFlags status_flags(const std::string& window_name, const std::vector<std::string>& path_to_element) const;
            ImGuiItemFlags item_flags(const std::string& window_name, const std::vector<std::string>& path_to_element) const;
            std::string item_text(const std::string& window_name, const std::vector<std::string>& path_to_element) const;
            Colour style_colour(const std::string& window_name, const std::vector<std::string>& path_to_element, ImGuiCol colour) const;

            void render();
            void render(const std::function<void()>& pre_render_callback);

            void reset();
            ImGuiWindow* find_window(const std::string& name) const;
            ImGuiID get_id(ImGuiWindow* window, const std::vector<std::string>& path_to_element) const;
            std::string popup_name(const std::string& name) const;
        private:
            Window _window;
            graphics::Device _device;
            RenderCallback _render_callback;
            std::unordered_map<ImGuiID, ImRect> _element_rects;
            std::unordered_map<ImGuiID, ImGuiItemStatusFlags> _status_flags;
            std::unordered_map<ImGuiID, ImGuiItemFlags> _item_flags;
            std::unordered_map<ImGuiID, std::array<ImVec4, ImGuiCol_COUNT>> _item_colours;
            std::unordered_map<ImGuiID, std::string> _item_text;
            ImGuiID _tracking_id{ 0 };
            ImGuiContext* _context{ nullptr };
        };
    }
}
