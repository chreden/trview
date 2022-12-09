#include <trview.app/Application.h>
#include <external/imgui/imgui_internal.h>

void ImGuiTestEngineHook_ItemInfo(ImGuiContext*, ImGuiID, const char*, ImGuiItemStatusFlags)
{
}

void ImGuiTestEngineHook_ItemAdd(ImGuiContext*, const ImRect&, ImGuiID)
{
}

void ImGuiTestEngineHook_Log(ImGuiContext*, const char*, ...)
{
}

const char* ImGuiTestEngine_FindItemDebugLabel(ImGuiContext*, ImGuiID)
{
    return "";
}

void ImGuiTrviewTestEngineHook_ItemText(ImGuiContext*, ImGuiID, const char*)
{
}

void ImGuiTrviewTestEngineHook_RenderedText(ImGuiContext*, ImGuiID, const char*)
{
}


int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int nCmdShow){
    auto application = trview::create_application(hInstance, nCmdShow, GetCommandLine());
    return application->run();
}
