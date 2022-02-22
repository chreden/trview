#include <trview.app/Application.h>
#include <external/imgui/imgui_internal.h>

void ImGuiTestEngineHook_ItemInfo(ImGuiContext* ctx, ImGuiID id, const char* label, ImGuiItemStatusFlags flags)
{
}

void ImGuiTestEngineHook_ItemAdd(ImGuiContext* ctx, const ImRect& bb, ImGuiID id)
{
}

void ImGuiTestEngineHook_Log(ImGuiContext* ctx, const char* fmt, ...)
{
}

const char* ImGuiTestEngine_FindItemDebugLabel(ImGuiContext* ctx, ImGuiID id)
{
    return "";
}

void ImGuiTrviewTestEngineHook_ItemText(ImGuiContext* ctx, ImGuiID id, const char*)
{
}

void ImGuiTrviewTestEngineHook_RenderedText(ImGuiContext* ctx, ImGuiID id, const char*)
{
}


int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int nCmdShow)
{
    auto window = trview::create_window(hInstance, nCmdShow);
    auto application = trview::create_application(window, GetCommandLine());
    return application->run();
}
