#include "TestImgui.h"
#include <trview.tests.common/Window.h>

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

        void TestImgui::click_element(const std::string& window_name, const std::vector<std::string>& path_to_element, bool show_context_menu, std::string hover)
        {
            const auto click_on_element = [&]()
            {
                auto window = find_window(window_name);
                if (hover != "")
                {
                    _context->HoveredWindow = find_window(hover);
                }
                else
                {
                    _context->HoveredWindow = window;
                }
                const auto id = get_id(window, path_to_element);
                _tracking_id = id;
                const auto bb = _element_rects[id];
                _context->HoveredId = id;
                _context->IO.MousePos = ImVec2(bb.Min.y, bb.Min.y);
                _context->IO.MouseClicked[0] = true;

                if (show_context_menu)
                {
                    _context->IO.MouseReleased[0] = true;
                    _context->IO.MouseDownDurationPrev[0] = _context->IO.KeyRepeatDelay;
                    _context->ActiveId = id;
                    _context->CurrentWindow = find_window("Debug##Default");
                    _context->MouseViewport = window->Viewport;
                }
            };
            render(click_on_element);

            _tracking_id = 0;
            _context->IO.MouseClicked[0] = false;
            _context->IO.MouseReleased[1] = false;
        }

        void TestImgui::click_element(TestImGuiId test_id, bool show_context_menu, bool hover)
        {
            const auto window = test_id.root();
            const auto id = test_id.id();
            const auto click_on_element = [&]()
            {
                _context->HoveredWindow = hover ? find_window(test_id.name()) : window;
                _tracking_id = id;
                const auto bb = _element_rects[id];
                _context->HoveredId = id;
                _context->IO.MousePos = ImVec2(bb.Min.y, bb.Min.y);
                _context->IO.MouseClicked[0] = true;

                if (show_context_menu)
                {
                    _context->IO.MouseReleased[0] = true;
                    _context->IO.MouseDownDurationPrev[0] = _context->IO.KeyRepeatDelay;
                    _context->ActiveId = id;
                    _context->CurrentWindow = find_window("Debug##Default");
                    _context->MouseViewport = window->Viewport;
                }
            };
            render(click_on_element);

            _tracking_id = 0;
            _context->IO.MouseClicked[0] = false;
            _context->IO.MouseReleased[1] = false;
        }

        void TestImgui::enter_text(const std::string& window_name, const std::vector<std::string>& path_to_element, const std::string& text)
        {
            const auto focus_on_element = [&]()
            {
                auto window = find_window(window_name);
                _context->HoveredWindow = window;
                const auto id = get_id(window, path_to_element);
                _tracking_id = id;
                const auto bb = _element_rects[id];
                _context->ActiveId = 0;
                _context->HoveredId = id;
                _context->IO.MousePos = ImVec2(bb.Min.y, bb.Min.y);
                _context->IO.MouseClicked[0] = true;
                for (const auto& c : text)
                {
                    _context->IO.InputQueueCharacters.push_back(c);
                }
            };

            const auto type_into_element = [&]()
            {
                _context->IO.MouseClicked[0] = false;
                _context->IO.KeysData[ImGuiKey_Enter].Down = true;
                _context->IO.KeysData[ImGuiKey_Enter].DownDuration = 0.5f;
                _context->IO.DeltaTime = 1.0f;
            };

            render(focus_on_element);
            render(type_into_element);

            _tracking_id = 0;
        }

        ImGuiID TestImgui::get_id(ImGuiWindow* window, const std::vector<std::string>& path_to_element) const
        {
            if (path_to_element.empty())
            {
                return 0;
            }

            auto previous_window = _context->CurrentWindow;
            _context->CurrentWindow = window;
            for (uint32_t i = 0; i < path_to_element.size() - 1; ++i)
            {
                ImGui::PushID(path_to_element[i].c_str());
            }

            const auto id = window->GetID(path_to_element.back().c_str());
            for (uint32_t i = 0; i < path_to_element.size() - 1; ++i)
            {
                ImGui::PopID();
            }

            _context->CurrentWindow = previous_window;
            return id;
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

        void TestImgui::add_style_colours(ImGuiID id, const std::array<ImVec4, ImGuiCol_COUNT>& colours)
        {
            _item_colours[id] = colours;
        }

        ImGuiItemStatusFlags TestImgui::status_flags(const std::string& window_name, const std::vector<std::string>& path_to_element) const
        {
            const auto window = find_window(window_name);
            const auto id = get_id(window, path_to_element);
            return _status_flags.find(id)->second;
        }

        ImGuiItemStatusFlags TestImgui::status_flags(TestImGuiId id) const
        {
            return _status_flags.find(id.id())->second;
        }

        ImGuiItemStatusFlags TestImgui::item_flags(const std::string& window_name, const std::vector<std::string>& path_to_element) const
        {
            const auto window = find_window(window_name);
            const auto id = get_id(window, path_to_element);
            return _item_flags.find(id)->second;
        }

        ImGuiItemFlags TestImgui::item_flags(TestImGuiId id) const
        {
            return _item_flags.find(id.id())->second;
        }

        std::string TestImgui::item_text(const std::string& window_name, const std::vector<std::string>& path_to_element) const
        {
            const auto window = find_window(window_name);
            const auto id = get_id(window, path_to_element);
            return _item_text.find(id)->second;
        }

        void TestImgui::render(const std::function<void()>& pre_render_callback)
        {
            _backend.new_frame();
            ImGui::NewFrame();
            pre_render_callback();
            if (_render_callback)
            {
                _render_callback();
            }
            ImGui::Render();
        }

        void TestImgui::render()
        {
            render([](){});
        }

        Colour TestImgui::style_colour(const std::string& window_name, const std::vector<std::string>& path_to_element, ImGuiCol colour) const
        {
            const auto window = find_window(window_name);
            const auto id = get_id(window, path_to_element);
            const auto& colours = _item_colours.find(id)->second;
            const auto result = colours[colour];
            return Colour(result.w, result.x, result.y, result.z);
        }

        Colour TestImgui::style_colour(TestImGuiId id, ImGuiCol colour) const
        {
            const auto& colours = _item_colours.find(id.id())->second;
            const auto result = colours[colour];
            return Colour(result.w, result.x, result.y, result.z);
        }

        bool TestImgui::element_present(const std::string& window_name, const std::vector<std::string>& path_to_element) const
        {
            const auto window = find_window(window_name);
            const auto id = get_id(window, path_to_element);
            return _element_rects.find(id) != _element_rects.end();
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

        std::string TestImgui::popup_name(const std::string& name) const
        {
            auto id = get_id(find_window("Debug##Default"), { name });
            std::stringstream stream;
            stream << "##Popup_" << std::hex << std::setfill('0') << std::setw(8) << id;
            return stream.str();
        }

        std::string TestImgui::child_name(const std::string& window_name, const std::vector<std::string>& child_path) const
        {
            std::stringstream stream;
            stream << window_name;

            std::string previous_name = window_name;
            for (const auto& child : child_path)
            {
                stream << '/' << child << '_' << std::hex << std::uppercase << std::setfill('0') << std::setw(8) << get_id(find_window(previous_name), { child });
                previous_name = stream.str();
            }
            return stream.str();
        }

        TestImGuiId TestImgui::id(const std::string& window_name) const
        {
            auto window = find_window(window_name);
            return TestImGuiId(window);
        }

        TestImGuiId TestImGuiId::push_child(const std::string& name)
        {
            auto context = ImGui::GetCurrentContext();
            auto id = _window->GetID(name.c_str());

            std::stringstream stream;
            stream << _window->Name << '/' << name << '_' << std::hex << std::uppercase << std::setfill('0') << std::setw(8) << id;
            auto child_name = stream.str();

            for (const auto& window : context->Windows)
            {
                if (window->Name == child_name)
                {
                    return TestImGuiId(window);
                }
            }
            throw 0;
        }

        TestImGuiId TestImGuiId::push_override(const std::string& name)
        {
            auto id = _window->GetID(name.c_str());
            _window->IDStack.push_back(id);
            return *this;
        }

        TestImGuiId TestImGuiId::push(const std::string& name)
        {
            auto new_id = _window->GetID(name.c_str());
            std::stringstream stream;
            stream << _window->Name << '/' << name << '_' << std::hex << std::uppercase << std::setfill('0') << std::setw(8) << new_id;
            _name = stream.str();
            _window->IDStack.push_back(new_id);
            return *this;
        }

        TestImGuiId::TestImGuiId(ImGuiWindow* window)
            : _window(window), _id(window->ID), _root_window(window)
        {
        }

        TestImGuiId TestImGuiId::id(const std::string& name)
        {
            _id = _window->GetID(name.c_str());
            while (_window->IDStack.size() > 1)
            {
                _window->IDStack.pop_back();
            }
            return *this;
        }

        ImGuiID TestImGuiId::id() const
        {
            return _id;
        }

        std::string TestImGuiId::name() const
        {
            return _name;
        }

        ImGuiWindow* TestImGuiId::root() const
        {
            return _root_window;
        }
    }
}