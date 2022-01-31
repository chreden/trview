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

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int nCmdShow)
{
    auto application = trview::create_application(hInstance, GetCommandLine(), nCmdShow);
    return application->run();
}
