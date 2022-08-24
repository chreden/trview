#include "TestImgui.h"
#include <trview.tests.common/Window.h>
#include <trview.common/Strings.h>
#include <format>

using namespace trview::tests;

void ImGuiTestEngineHook_ItemInfo(ImGuiContext* ctx, ImGuiID id, const char* label, ImGuiItemStatusFlags flags)
{
    TestImgui* test = static_cast<TestImgui*>(ctx->TestEngine);
    if (!test)
    {
        return;
    }

    test->add_status_flag(id, flags);
    test->add_item_flag(id, ctx->CurrentItemFlags);

    std::array<ImVec4, ImGuiCol_COUNT> colours;
    memcpy(&colours[0], ctx->Style.Colors, sizeof(ctx->Style.Colors));
    test->add_style_colours(id, colours);
}

void ImGuiTestEngineHook_ItemAdd(ImGuiContext* ctx, const ImRect& bb, ImGuiID id)
{
    TestImgui* test = static_cast<TestImgui*>(ctx->TestEngine);
    if (!test)
    {
        return;
    }

    test->add_rect(id, bb);
    test->add_item_flag(id, ctx->CurrentItemFlags);

    std::array<ImVec4, ImGuiCol_COUNT> colours;
    memcpy(&colours[0], ctx->Style.Colors, sizeof(ctx->Style.Colors));
    test->add_style_colours(id, colours);
}

void ImGuiTestEngineHook_Log(ImGuiContext* ctx, const char* fmt, ...)
{

}

const char* ImGuiTestEngine_FindItemDebugLabel(ImGuiContext* ctx, ImGuiID id)
{
    return "";
}

void ImGuiTrviewTestEngineHook_ItemText(ImGuiContext* ctx, ImGuiID id, const char* buf)
{
    TestImgui* test = static_cast<TestImgui*>(ctx->TestEngine);
    if (!test)
    {
        return;
    }

    test->add_item_text(id, buf);
}

void ImGuiTrviewTestEngineHook_RenderedText(ImGuiContext* ctx, ImGuiID id, const char* buf)
{
    TestImgui* test = static_cast<TestImgui*>(ctx->TestEngine);
    if (!test)
    {
        return;
    }

    test->add_rendered_text(id, buf);
}

namespace trview
{
    namespace tests
    {
        TestImgui::TestImgui()
        {
            _context = ImGui::CreateContext();
            _context->TestEngineHookItems = true;
            _context->TestEngine = this;
            ImGuiIO& io = ImGui::GetIO();
            io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
            io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
            io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
            if (io.BackendPlatformUserData == 0)
            {
                _backend.initialise();
            }

            wchar_t* path = 0;
            SHGetKnownFolderPath(FOLDERID_Fonts, 0, nullptr, &path);
            _font = io.Fonts->AddFontFromFileTTF((to_utf8(path) + "\\Arial.ttf").c_str(), 12.0f);
            CoTaskMemFree(path);
        }

        TestImgui::TestImgui(const RenderCallback& render_callback)
            : TestImgui()
        {
            _render_callback = render_callback;
            render();
        }

        TestImgui::~TestImgui()
        {
            ImGui::DestroyContext(_context);
        }

        void TestImgui::click_element_internal(TestImGuiId test_id, bool show_context_menu, TestImGuiId active_override, bool hover)
        {
            const auto window = find_window(test_id.root());
            const auto id = test_id.id();
            const auto click_on_element = [&]()
            {
                _context->HoveredWindow = hover ? find_window(test_id.hover()) : window;
                _tracking_id = id;
                const auto bb = _element_rects[id];
                _context->HoveredId = id;
                _context->IO.MousePos = ImVec2(bb.Min.y, bb.Min.y);
                _context->IO.MouseClicked[0] = true;

                if (active_override.id() != 0)
                {
                    _context->ActiveId = active_override.id();
                }

                if (show_context_menu)
                {
                    _context->IO.MouseReleased[0] = true;
                    _context->IO.MouseDownDurationPrev[0] = _context->IO.KeyRepeatDelay;

                    _context->CurrentWindow = find_window("Debug##Default");
                    _context->MouseViewport = window->Viewport;
                }
            };
            render(click_on_element);

            _tracking_id = 0;
            _context->IO.MouseClicked[0] = false;
            _context->IO.MouseReleased[1] = false;
        }

        void TestImgui::click_element(TestImGuiId test_id, bool show_context_menu, TestImGuiId active_override)
        {
            click_element_internal(test_id, show_context_menu, active_override, false);
        }

        void TestImgui::click_element_with_hover(TestImGuiId test_id, bool show_context_menu, TestImGuiId active_override)
        {
            click_element_internal(test_id, show_context_menu, active_override, true);
        }

        void TestImgui::hover_element(TestImGuiId test_id)
        {
            const auto window = find_window(test_id.root());
            const auto id = test_id.id();
            const auto hover_on_element = [&]()
            {
                _context->HoveredWindow = window;
                _tracking_id = id;
                const auto bb = _element_rects[id];
                _context->HoveredId = id;
                _context->IO.MousePos = ImVec2(bb.Min.y, bb.Min.y);
            };
            render(hover_on_element);
            _tracking_id = 0;
        }

        void TestImgui::mouse_down_element(TestImGuiId test_id, TestImGuiId active_override)
        {
            const auto window = find_window(test_id.root());
            const auto id = test_id.id();
            const auto click_on_element = [&]()
            {
                _context->HoveredWindow = window;
                _tracking_id = id;
                const auto bb = _element_rects[id];
                _context->HoveredId = id;
                _context->IO.MousePos = ImVec2(bb.Min.y, bb.Min.y);
                _context->IO.MouseDown[0] = true;

                if (active_override.id() != 0)
                {
                    _context->ActiveId = active_override.id();
                }
            };
            render(click_on_element);

            _tracking_id = 0;
            _context->IO.MouseDown[0] = false;
        }

        void TestImgui::enter_text(const std::string& text)
        {
            const auto type_into_element = [&]()
            {
                for (const auto& c : text)
                {
                    _context->IO.InputQueueCharacters.push_back(c);
                }
            };

            render(type_into_element);

            _tracking_id = 0;
        }

        void TestImgui::press_key(ImGuiKey key)
        {
            const auto type_into_element = [&]()
            {
                _context->IO.KeysData[key].Down = true;
                _context->IO.KeysData[key].DownDuration = 0.5f;
                _context->IO.DeltaTime = 1.0f;
            };

            render(type_into_element);
        }

        ImGuiWindow* TestImgui::find_window(ImGuiID id) const
        {
            for (const auto& w : _context->Windows)
            {
                if (w->ID == id)
                {
                    return w;
                }
            }
            return nullptr;
        }

        ImGuiWindow* TestImgui::find_window(const std::string& name) const
        {
            for (const auto& w : _context->Windows)
            {
                if (std::string(w->Name) == name)
                {
                    return w;
                }
            }
            return nullptr;
        }

        void TestImgui::add_rect(ImGuiID id, const ImRect& rect)
        {
            _element_rects[id] = rect;
            if (id == _tracking_id)
            {
                _context->IO.MousePos = ImVec2(rect.Min.x, rect.Min.y) + ImVec2(4, 4);
            }
        }

        void TestImgui::add_status_flag(ImGuiID id, ImGuiItemStatusFlags flags)
        {
            _status_flags[id] = flags;
        }

        void TestImgui::add_item_flag(ImGuiID id, ImGuiItemFlags flags)
        {
            _item_flags[id] = flags;
        }

        void TestImgui::add_item_text(ImGuiID id, const std::string& text)
        {
            _item_text[id] = text;
        }

        void TestImgui::add_rendered_text(ImGuiID id, const std::string& text)
        {
            _item_rendered_text[id].push_back(text);
        }

        void TestImgui::add_style_colours(ImGuiID id, const std::array<ImVec4, ImGuiCol_COUNT>& colours)
        {
            _item_colours[id] = colours;
        }

        ImGuiItemStatusFlags TestImgui::status_flags(TestImGuiId id) const
        {
            return _status_flags.find(id.id())->second;
        }

        ImGuiItemFlags TestImgui::item_flags(TestImGuiId id) const
        {
            return _item_flags.find(id.id())->second;
        }

        std::string TestImgui::item_text(TestImGuiId id) const
        {
            return _item_text.find(id.id())->second;
        }

        std::vector<std::string> TestImgui::rendered_text(TestImGuiId id) const
        {
            return _item_rendered_text.find(id.id())->second;
        }

        void TestImgui::render(const std::function<void()>& pre_render_callback)
        {
            _backend.new_frame();
            ImGui::NewFrame();
            ImGui::PushFont(_font);
            pre_render_callback();
            if (_render_callback)
            {
                _render_callback();
            }
            ImGui::PopFont();
            ImGui::Render();
            
        }

        void TestImgui::render()
        {
            render([](){});
        }

        Colour TestImgui::style_colour(TestImGuiId id, ImGuiCol colour) const
        {
            const auto& colours = _item_colours.find(id.id())->second;
            const auto result = colours[colour];
            return Colour(result.w, result.x, result.y, result.z);
        }

        bool TestImgui::element_present(TestImGuiId id) const
        {
            return _element_rects.find(id.id()) != _element_rects.end();
        }

        void TestImgui::reset()
        {
            _element_rects.clear();
            _status_flags.clear();
            _item_flags.clear();
            _item_colours.clear();
            _item_text.clear();
            _item_rendered_text.clear();
            _tracking_id = 0;
        }

        void TestImgui::show_context_menu(const std::string& window_name)
        {
            const auto click_on_element = [&]()
            {
                auto window = find_window(window_name);
                _context->HoveredWindow = nullptr;
                _context->CurrentWindow = window;
                _context->IO.MouseReleased[1] = true;
            };
            render(click_on_element);

            _tracking_id = 0;
            _context->IO.MouseClicked[0] = false;
            _context->IO.MouseReleased[1] = false;
        }

        TestImGuiId TestImgui::id(const std::string& window_name) const
        {
            return TestImGuiId(window_name);
        }

        TestImGuiId TestImgui::popup_id(const std::string& popup_name) const
        {
            auto id = TestImGuiId("Debug##Default").id(popup_name).id();
            return TestImGuiId(std::format("##Popup_{:0>8x}", id));
        }
    }
}