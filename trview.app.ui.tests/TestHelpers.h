#pragma once

#include <string>
#include <imgui_te_context.h>

namespace trview
{
    template <typename T>
    void test(ImGuiTestEngine* engine, const char* category, const char* name, ImFuncPtr(ImGuiTestGuiFunc) gui_func, ImFuncPtr(ImGuiTestTestFunc) test_func);

    std::string ItemText(ImGuiTestContext* ctx, ImGuiTestRef ref);
    std::string RenderedText(ImGuiTestContext* ctx, ImGuiTestRef ref);
}

#include "TestHelpers.inl"
