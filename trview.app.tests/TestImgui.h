#pragma once

#define IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_ENABLE_TEST_ENGINE
#include <external/imgui/imgui.h>
#include <external/imgui/imgui_internal.h>
#include "NullImGuiBackend.h"
#include "TestImGuiId.h"

#include <unordered_map>

namespace trview
{
    namespace tests
    {
        class TestImgui final
        {
        public:
            using RenderCallback = std::function<void()>;

            explicit TestImgui();
            explicit TestImgui(const RenderCallback& callback);
            ~TestImgui();

            void click_element(TestImGuiId id, bool show_context_menu = false, TestImGuiId active_override = TestImGuiId());
            void click_element_with_hover(TestImGuiId id, TestImGuiId hover_id, bool show_context_menu = false, TestImGuiId active_override = TestImGuiId());
            void enter_text(const std::string& text);
            void press_key(ImGuiKey key);

            /// <summary>
            /// Add a control recangle.
            /// </summary>
            /// <param name="id">The ID of the control.</param>
            /// <param name="rect">The latest bounding rectangle.</param>
            void add_rect(ImGuiID id, const ImRect& rect);
            void add_status_flag(ImGuiID id, ImGuiItemStatusFlags flags);
            void add_item_flag(ImGuiID id, ImGuiItemFlags flags);
            void add_item_text(ImGuiID id, const std::string& text);
            void add_rendered_text(ImGuiID id, const std::string& text);
            void add_style_colours(ImGuiID id, const std::array<ImVec4, ImGuiCol_COUNT>& colours);
            bool element_present(TestImGuiId id) const;
            ImGuiItemStatusFlags status_flags(TestImGuiId id) const;
            ImGuiItemFlags item_flags(TestImGuiId id) const;
            std::string item_text(TestImGuiId id) const;
            std::vector<std::string> rendered_text(TestImGuiId id) const;
            Colour style_colour(TestImGuiId id, ImGuiCol colour) const;
            void show_context_menu(const std::string& window_name);
            void render();
            void render(const std::function<void()>& pre_render_callback);
            void reset();
            ImGuiWindow* find_window(const std::string& name) const;
            ImGuiWindow* find_window(ImGuiID id) const;
            TestImGuiId id(const std::string& window_name) const;
            TestImGuiId popup_id(const std::string& popup_name) const;
        private:
            RenderCallback _render_callback;
            std::unordered_map<ImGuiID, ImRect> _element_rects;
            std::unordered_map<ImGuiID, ImGuiItemStatusFlags> _status_flags;
            std::unordered_map<ImGuiID, ImGuiItemFlags> _item_flags;
            std::unordered_map<ImGuiID, std::array<ImVec4, ImGuiCol_COUNT>> _item_colours;
            std::unordered_map<ImGuiID, std::string> _item_text;
            std::unordered_map<ImGuiID, std::vector<std::string>> _item_rendered_text;
            ImGuiID _tracking_id{ 0 };
            ImGuiContext* _context{ nullptr };
            NullImGuiBackend _backend;
        };
    }
}
