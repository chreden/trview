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

namespace trview
{
    namespace tests
    {
        TestImgui::TestImgui(const RenderCallback& render_callback)
            : _window(create_test_window(L"Test")), _render_callback(render_callback)
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
                ImGui_ImplWin32_Init(_window);
                ImGui_ImplDX11_Init(_device.device().Get(), _device.context().Get());
                auto f = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Arial.ttf", 12.0f);
            }
            render();
        }

        TestImgui::~TestImgui()
        {
            ImGui_ImplDX11_Shutdown();
            ImGui_ImplWin32_Shutdown();
            ImGui::DestroyContext(_context);
        }

        void TestImgui::click_element(const std::string& window_name, const std::vector<std::string>& path_to_element)
        {
            const auto setup = [&]()
            {
                auto window = find_window(window_name);
                _context->HoveredWindow = window;
                const auto id = get_id(window, path_to_element);
                _tracking_id = id;
                const auto bb = _element_rects[id];
                _context->HoveredId = id;
                _context->IO.MousePos = ImVec2(bb.Min.y, bb.Min.y);
                _context->IO.MouseClicked[0] = true;
            };
            render(setup);

            _context->IO.MouseClicked[0] = false;
        }

        ImGuiID TestImgui::get_id(ImGuiWindow* window, const std::vector<std::string>& path_to_element) const
        {
            if (path_to_element.empty())
            {
                return 0;
            }

            _context->CurrentWindow = window;
            for (uint32_t i = 0; i < path_to_element.size() - 1; ++i)
            {
                ImGui::PushID(path_to_element[i].c_str());
            }

            auto id = window->GetID(path_to_element.back().c_str());

            for (uint32_t i = 0; i < path_to_element.size() - 1; ++i)
            {
                ImGui::PopID();
            }

            _context->CurrentWindow = nullptr;
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
                _context->IO.MousePos = ImVec2(rect.Min.x, rect.Min.y);
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

        ImGuiItemStatusFlags TestImgui::item_flags(const std::string& window_name, const std::vector<std::string>& path_to_element) const
        {
            const auto window = find_window(window_name);
            const auto id = get_id(window, path_to_element);
            return _item_flags.find(id)->second;
        }

        void TestImgui::render(const std::function<void()>& pre_render_callback)
        {
            ImGui_ImplDX11_NewFrame();
            ImGui_ImplWin32_NewFrame();
            ImGui::NewFrame();
            pre_render_callback();
            _render_callback();
            ImGui::Render();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
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

        bool TestImgui::element_present(const std::string& window_name, const std::vector<std::string>& path_to_element) const
        {
            const auto window = find_window(window_name);
            const auto id = get_id(window, path_to_element);
            return _element_rects.find(id) != _element_rects.end();
        }

        void TestImgui::reset()
        {
            _element_rects.clear();
            _status_flags.clear();
            _item_flags.clear();
            _item_colours.clear();
            _tracking_id = 0;
        }
    }
}